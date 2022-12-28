#include "gtest/gtest.h"
#include "kqueue_handler.hpp"
#include "server_socket.hpp"
#include "udata.hpp"
#include "client_socket.hpp"
#include "event_executor.hpp"

# define BUFFER_SIZE 1024

namespace {
class SendResponseTest : public ::testing::Test {
   protected:
	void CreateServerSocket(std::vector<ServerInfo> server_infos) {
		ServerInfo server_info;
		server_info.SetHost("127.0.0.1");
		server_info.SetPort("8080");
		server_infos.push_back(server_info);
		server_socket_ = new ServerSocket(server_infos); // Listen
	}

	void CreateClientSocket() {
		client_fd = socket(AF_INET, SOCK_STREAM, 0);

		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		addr.sin_port = htons(8080);

		int res = connect(client_fd, (struct sockaddr *) &addr, sizeof(addr));
		EXPECT_NE(res, -1);
	}

	ResponseMessage CreateResponse(int status_code, const std::string &reason_phrase, const std::string &body) {
		ResponseMessage response_message(status_code, reason_phrase);
		response_message.AppendBody(body);
		response_message.CalculateLength();
		return response_message;
	}

	std::string RecvResponse() const {
		char buf[BUFFER_SIZE + 1];
		std::string response;

		while (true) {
			int count = recv(client_fd, buf, BUFFER_SIZE, 0);
			if (count <= 0) {
				break;
			}
			response.append(buf, count);
		}
		return response;
	}

	virtual void SetUp() {
		kqueue_handler_ = new KqueueHandler();

		std::vector<ServerInfo> server_infos;
		CreateServerSocket(server_infos); // 127.0.0.1 8080

		Udata *udata = new Udata(Udata::LISTEN, server_socket_->GetSocketDescriptor());
		kqueue_handler_->AddReadEvent(server_socket_->GetSocketDescriptor(), udata); // Kqueue에 Listen 등록

		CreateClientSocket(); // connect
		struct kevent event = kqueue_handler_->MonitorEvent();
		Udata *user_data = reinterpret_cast<Udata *>(event.udata);
		ASSERT_EQ(user_data->state_, Udata::LISTEN);

		accepted_fd = server_socket_->AcceptClient();
		client_socket_ = new ClientSocket(accepted_fd, server_socket_->GetServerInfos());
		Udata *client_udata = new Udata(Udata::SEND_RESPONSE, client_socket_->GetSocketDescriptor());
		kqueue_handler_->AddWriteEvent(client_socket_->GetSocketDescriptor(), client_udata);

	}

	virtual void TearDown() {
		delete kqueue_handler_;
		close(server_socket_->GetSocketDescriptor());
		close(accepted_fd);
		close(client_fd);
		close(client_socket_->GetSocketDescriptor());
		delete server_socket_;
		delete client_socket_;
	}

	KqueueHandler *kqueue_handler_;
	ServerSocket *server_socket_;
	ClientSocket *client_socket_;
	int accepted_fd;
	int client_fd;
};

TEST_F(SendResponseTest, send_response) {
	struct kevent event = kqueue_handler_->MonitorEvent();

	ResponseMessage response_message = CreateResponse(200, "OK", "Hello World!");

	RequestMessage request_message(1000);
	std::string key = "connection";
	for (char c : key) {
		request_message.AppendHeaderName(c);
	}
	std::string value = "close";
	for (char c : value) {
		request_message.AppendHeaderValue(c);
	}
	request_message.AddHeaderField();

	Udata *udata = reinterpret_cast<Udata *>(event.udata);
	EXPECT_EQ(udata->state_, Udata::SEND_RESPONSE);
	udata->request_message_ = request_message;
	udata->response_message_ = response_message;

	EXPECT_EQ(event.ident, client_socket_->GetSocketDescriptor());
	EXPECT_EQ(event.filter, EVFILT_WRITE);

	int result = EventExecutor::SendResponse(client_socket_->GetSocketDescriptor(), udata);
	if (result == Udata::CLOSE) {
		close(client_socket_->GetSocketDescriptor());
	}

	std::string response = RecvResponse();
	std::cerr << response << std::endl;
}
}
