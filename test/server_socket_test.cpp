//
// Created by 김민준 on 2022/12/21.
//

#include <arpa/inet.h>
#include "gtest/gtest.h"
#include "server_socket.hpp"
#include "core_exception.h"

namespace {
class ServerSocketTest : public ::testing::Test {
   public:
	ServerInfo CreateServerInfo(const std::string& host, const std::string& port) {
		ServerInfo server_info;
		server_info.SetPort(host);
		server_info.SetPort(port);
		return server_info;
	}

   protected:
	std::vector<ServerInfo> server_infos_;
};

TEST_F(ServerSocketTest, socket_member_test) {
	ServerInfo server_info = CreateServerInfo("127.0.0.1", "8080");
	server_infos_.push_back(server_info);

	ServerSocket server_socket(server_infos_);
	EXPECT_EQ(server_socket.GetType(), Socket::SERVER_TYPE);
	EXPECT_GT(server_socket.GetSocketDescriptor(), 0);
}

TEST_F(ServerSocketTest, normal_create) {
	ServerInfo server_info = CreateServerInfo("127.0.0.1", "8080");
	server_infos_.push_back(server_info);

	EXPECT_NO_THROW(ServerSocket server_socket(server_infos_));

	server_info.SetPort("8090");
	EXPECT_NO_THROW(ServerSocket server_socket(server_infos_));

	server_info.SetPort("8100");
	EXPECT_NO_THROW(ServerSocket server_socket(server_infos_));
}

TEST_F(ServerSocketTest, bind_exception) {
	ServerInfo server_info = CreateServerInfo("127.0.0.1", "8080");
	server_infos_.push_back(server_info);

	EXPECT_THROW(ServerSocket server_socket1(server_infos_);
					 ServerSocket server_socket2(server_infos_), CoreException::BindException);
}

TEST_F(ServerSocketTest, accept_client) {
	ServerInfo server_info = CreateServerInfo("127.0.0.1", "8080");
	server_infos_.push_back(server_info);

	ServerSocket *server_socket;
	ASSERT_NO_THROW(server_socket = new ServerSocket(server_infos_));

	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	ASSERT_GT(client_fd, 0);

	sockaddr_in server_addr = {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(8080);

	int result = connect(client_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	ASSERT_NE(result, -1);

	int accepted_fd = server_socket->AcceptClient();
	ASSERT_GT(accepted_fd, 0);

	delete server_socket;
}
}
