#include "gtest/gtest.h"
#include "kqueue_handler.hpp"
#include "server_socket.hpp"
#include "udata.hpp"
#include "client_socket.hpp"
#include "event_executor.hpp"
#include "character_const.hpp"

# define BUFFER_SIZE 1024

namespace {
class SendResponseTest : public ::testing::Test {
   protected:
	virtual void SetUp() {
		kqueue_handler_ = new KqueueHandler();

		std::vector<ServerInfo> server_infos;
		server_infos.push_back(CreateServerInfo());
		server_socket_ = new ServerSocket(server_infos);

		Udata *udata = new Udata(Udata::LISTEN, server_socket_->GetSocketDescriptor());
		kqueue_handler_->AddReadEvent(server_socket_->GetSocketDescriptor(), udata); // Kqueue에 Listen 등록

		CreateClientSocket(); // connect
		struct kevent event = kqueue_handler_->MonitorEvent();
		Udata *user_data = reinterpret_cast<Udata *>(event.udata);
		ASSERT_EQ(event.ident, server_socket_->GetSocketDescriptor());
		ASSERT_EQ(user_data->state_, Udata::LISTEN);

		accepted_socket_ = server_socket_->AcceptClient();
		accepted_socket_->SetServerInfo(server_socket_->GetServerInfos()[0]);
		Udata *client_udata = new Udata(Udata::SEND_RESPONSE, accepted_socket_->GetSocketDescriptor());
		kqueue_handler_->AddWriteEvent(accepted_socket_->GetSocketDescriptor(), client_udata);
	}

	virtual void TearDown() {
		delete kqueue_handler_;
		close(server_socket_->GetSocketDescriptor());
		close(client_fd);
		delete server_socket_;
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

	ResponseMessage CreateResponse(int status_code, const std::string &reason_phrase, const std::string &body) {
		ResponseMessage response_message(status_code, reason_phrase);
		response_message.AppendBody(body);
		return response_message;
	}

	KqueueHandler *kqueue_handler_;
	ServerSocket *server_socket_;
	ClientSocket *accepted_socket_; // listen socket에서 accept한 client의 fd
	int client_fd; // connect할 client의 fd
   private:
	ServerInfo CreateServerInfo() {
		ServerInfo server_info;
		server_info.SetHost("127.0.0.1");
		server_info.SetPort("8080");
		std::string error_pages = "error_page 404 405 ./test/docs/error/error.html";
		server_info.SetErrorPages(error_pages);
		return server_info;
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

};

TEST_F(SendResponseTest, send_response) {
	struct kevent event = kqueue_handler_->MonitorEvent();

	ResponseMessage response_message = CreateResponse(200, "OK", "Hello World!");

	RequestMessage request_message(1000);
	request_message.SetConnection(false);

	Udata *udata = reinterpret_cast<Udata *>(event.udata);
	EXPECT_EQ(udata->state_, Udata::SEND_RESPONSE);
	udata->request_message_ = request_message;
	udata->response_message_ = response_message;

	EXPECT_EQ(event.ident, accepted_socket_->GetSocketDescriptor());
	EXPECT_EQ(event.filter, EVFILT_WRITE);

	EventExecutor::SendResponse(*kqueue_handler_, accepted_socket_, udata);
	delete accepted_socket_;

	std::string response = RecvResponse();
	std::string expected = response_message.ToString();
	ASSERT_EQ(expected, response);
}

TEST_F(SendResponseTest, send_error_response) {
	struct kevent event = kqueue_handler_->MonitorEvent();
	Udata *udata = reinterpret_cast<Udata *>(event.udata);
	EXPECT_EQ(udata->state_, Udata::SEND_RESPONSE);
	EXPECT_EQ(event.ident, accepted_socket_->GetSocketDescriptor());
	EXPECT_EQ(event.filter, EVFILT_WRITE);

	ResponseMessage response_message(NOT_FOUND, "NOT_FOUND");

	RequestMessage request_message(1000);
	request_message.SetConnection(false);

	udata->request_message_ = request_message;
	udata->response_message_ = response_message;
	udata->sock_d_ = accepted_socket_->GetSocketDescriptor();

	EventExecutor::SendResponse(*kqueue_handler_, accepted_socket_, udata);

	event = kqueue_handler_->MonitorEvent();
	// error page write event

	EventExecutor::ReadFile(*kqueue_handler_, event.ident, event.data, udata);

	event = kqueue_handler_->MonitorEvent(); // SEND_RESPONSE
	EXPECT_EQ(event.ident, accepted_socket_->GetSocketDescriptor());

	std::string expected = udata->response_message_.ToString();
	EventExecutor::SendResponse(*kqueue_handler_, accepted_socket_, udata);
	delete accepted_socket_;

	std::string response = RecvResponse();
	ASSERT_EQ(expected, response);
}

TEST_F(SendResponseTest, send_default_error) {
	struct kevent event = kqueue_handler_->MonitorEvent();
	Udata *udata = reinterpret_cast<Udata *>(event.udata);
	EXPECT_EQ(udata->state_, Udata::SEND_RESPONSE);
	EXPECT_EQ(event.ident, accepted_socket_->GetSocketDescriptor());
	EXPECT_EQ(event.filter, EVFILT_WRITE);
	accepted_socket_->SetServerInfo(ServerInfo());

	ResponseMessage response_message(NOT_FOUND, "NOT_FOUND");

	RequestMessage request_message(1000);
	request_message.SetConnection(false);

	udata->request_message_ = request_message;
	udata->response_message_ = response_message;
	udata->sock_d_ = accepted_socket_->GetSocketDescriptor();

	EventExecutor::SendResponse(*kqueue_handler_, accepted_socket_, udata);

	event = kqueue_handler_->MonitorEvent();
	EXPECT_EQ(event.ident, accepted_socket_->GetSocketDescriptor());

	delete accepted_socket_;

	std::string expected = "HTTP/1.1 404 NOT_FOUND" CRLF
						   "Content-Length: 203" CRLF
						   "Server: Webserv" DCRLF
						   "<!DOCTYPE html>"
						   "<html>"
						   "  <head>"
						   "    <title> Default Error Page </title>"
						   "  </head>"
						   "  <body>"
						   "    <h1> Page Not Found </h1>"
						   "    <p>The page you are looking for could not be found on our server.</p>"
						   "  </body>"
						   "</html>";
	std::string response = RecvResponse();
	EXPECT_EQ(expected, response);
}
}
