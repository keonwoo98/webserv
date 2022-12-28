//
// Created by 김민준 on 2022/12/22.
//

#include "kqueue_handler.hpp"
#include "gtest/gtest.h"
#include "server_socket.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace {
class KqueueHandlerTest : public ::testing::Test {
   protected:
	virtual void SetUp() {
		ASSERT_NO_THROW(kqueue_handler_ = new KqueueHandler());

		std::vector<ServerInfo> server_infos;
		ServerInfo server_info;
		server_info.SetPort("4242");
		server_info.SetHost("127.0.0.1");
		server_infos.push_back(server_info);

		server_socket_ = new ServerSocket(server_infos);
		client_sock_d_ = socket(AF_INET, SOCK_STREAM, 0);

		ASSERT_GT(server_socket_->GetSocketDescriptor(), 0);
		ASSERT_GT(client_sock_d_, 0);

		kqueue_handler_->AddReadEvent(server_socket_->GetSocketDescriptor(), NULL); // Kqueue에 Listen Read 이벤트 등록
	}

	virtual void TearDown() {
		delete kqueue_handler_;
		delete server_socket_;
		close(client_sock_d_);
	}

	KqueueHandler *kqueue_handler_ = nullptr;
	ServerSocket *server_socket_;
	int client_sock_d_;
};

TEST_F(KqueueHandlerTest, read_event) {
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	address.sin_port = htons(4242);

	int result = connect(client_sock_d_, (struct sockaddr *) &address, sizeof(address));
	EXPECT_NE(result, -1);

	struct kevent event = kqueue_handler_->MonitorEvent();
	EXPECT_EQ(event.ident, server_socket_->GetSocketDescriptor());
	EXPECT_EQ(event.filter, EVFILT_READ);
	EXPECT_EQ(event.flags, EV_ADD);
	EXPECT_GT(event.data, 0);
};
}


