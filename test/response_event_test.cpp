#include "gtest/gtest.h"
#include "kqueue_handler.hpp"
#include "server_socket.hpp"
#include "udata.hpp"
#include "client_socket.hpp"

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
		response_message.SetBody(body);
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

		Udata *udata = new Udata(LISTEN, server_socket_);
		kqueue_handler_->AddReadEvent(server_socket_->GetSocketDescriptor(), udata); // Kqueue에 Listen 등록

		CreateClientSocket(); // connect
		std::vector<struct kevent> events = kqueue_handler_->MonitorEvents();
		Udata *user_data = reinterpret_cast<Udata *>(events[0].udata);
		ServerSocket *event_socket = reinterpret_cast<ServerSocket *>(user_data->socket_);

		accepted_fd = event_socket->AcceptClient();
		client_socket_ = new ClientSocket(accepted_fd, event_socket->GetServerInfos());
		Udata *client_udata = new Udata(client_socket_->GetSocketDescriptor(), client_socket_);
		kqueue_handler_->AddWriteEvent(client_socket_->GetSocketDescriptor(), client_udata);
	}

	virtual void TearDown() {

	}

	KqueueHandler *kqueue_handler_;
	ServerSocket *server_socket_;
	ClientSocket *client_socket_;
	int accepted_fd;
	int client_fd;
};

TEST_F(SendResponseTest, send_response) {
	std::vector<struct kevent> events = kqueue_handler_->MonitorEvents();
	ResponseMessage response_message = CreateResponse(200, "OK", "Hello World!");
	client_socket_->SetResponse(response_message);
	RequestMessage request_message(1000);
	request_message.SetHeader(std::make_pair("connection", "close"));
	client_socket_->SetRequest(request_message);

	Udata *udata = reinterpret_cast<Udata *>(events[0].udata);
	ClientSocket *client_socket = reinterpret_cast<ClientSocket *>(udata->socket_);
	EXPECT_EQ(events[0].ident, client_socket->GetSocketDescriptor());
	EXPECT_EQ(events[0].filter, EVFILT_WRITE);

	client_socket_->SendResponse(*kqueue_handler_, udata);

	std::string response = RecvResponse();
	std::cerr << response << std::endl;
}
}
