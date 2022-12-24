//
// Created by 김민준 on 2022/12/22.
//

#include "kqueue_handler.hpp"
#include "gtest/gtest.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

class KqueueHandlerTest : public ::testing::Test {
   protected:

	int CreateSocket() {
		return socket(AF_INET, SOCK_STREAM, 0);
	}

	void InitializeAddress() {
		server_addr_.sin_family = AF_INET; // IPv4
		server_addr_.sin_port = htons(8080); // 8080
		server_addr_.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1
	}

	void BindSocket() {
		ASSERT_NE(bind(server_sock_d_, (struct sockaddr *) &server_addr_, sizeof(server_addr_)), -1);
	}

	void ListenSocket() const {
		ASSERT_NE(listen(server_sock_d_, 10), -1);
	}

	virtual void SetUp() {
		ASSERT_NO_THROW(kqueue_handler_ = new KqueueHandler());

		client_sock_d_ = CreateSocket();
		server_sock_d_ = CreateSocket();
		ASSERT_GT(client_sock_d_, 0);
		ASSERT_GT(server_sock_d_, 0);

		InitializeAddress();
		BindSocket();
		ListenSocket();
		kqueue_handler_->AddReadEvent(server_sock_d_, NULL);
	}

	virtual void TearDown() {
		delete kqueue_handler_;
		close(client_sock_d_);
		close(server_sock_d_);
	}

	struct sockaddr_in server_addr_ = {};
	KqueueHandler *kqueue_handler_ = nullptr;
	int client_sock_d_ = -1;
	int server_sock_d_ = -1;

};

TEST_F(KqueueHandlerTest, read_event_test) {
	int result = connect(client_sock_d_, (struct sockaddr *) &server_addr_, sizeof(server_addr_));
	EXPECT_NE(result, -1);

	std::string request = "/ HTTP/1.1\r\n"
						  "Host: localhost:8000\r\n\r\n";
	result = send(client_sock_d_, request.c_str(), request.length(), 0);
	EXPECT_NE(result, -1);

	std::vector<struct kevent> events = kqueue_handler_->MonitorEvents();
	EXPECT_EQ(events.size(), 1);
	EXPECT_EQ(events[0].ident, server_sock_d_);
	EXPECT_EQ(events[0].filter, EVFILT_READ);
	EXPECT_EQ(events[0].flags, EV_ADD);
	EXPECT_GT(events[0].data, 0);
}

TEST_F(KqueueHandlerTest, delete_event_test) {
	int result = connect(client_sock_d_, (struct sockaddr *) &server_addr_, sizeof(server_addr_));
	std::string request = "/ HTTP/1.1\r\n"
						  "Host: localhost:8000\r\n\r\n";
	EXPECT_NE(result, -1);

	result = send(client_sock_d_, request.c_str(), request.length(), 0);
	EXPECT_NE(result, -1);

	std::vector<struct kevent> events = kqueue_handler_->MonitorEvents();
	EXPECT_EQ(events.size(), 1);

	int accepted_fd = accept(server_sock_d_, NULL, NULL);
	EXPECT_GT(accepted_fd, 0);

	kqueue_handler_->AddReadEvent(accepted_fd, NULL); // accept한 fd를 READ 이벤트 등록
	kqueue_handler_->DeleteReadEvent(server_sock_d_, NULL); // Listen Event DELTE

	events = kqueue_handler_->MonitorEvents(); // accept한 fd만 나와야 한다.
	EXPECT_EQ(events.size(), 1);
	EXPECT_EQ(events[0].ident, accepted_fd);
}
