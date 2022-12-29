//
// Created by 김민준 on 2022/12/29.
//

#include "gtest/gtest.h"
#include "socket.hpp"

TEST(socket_addr, int_to_host) {
	uint32_t host = 16777343; // 127.0.0.1

	std::string ip = Socket::HostToIpAddr(host);
	std::string expected = "127.0.0.1";
	EXPECT_EQ(ip, expected);
}
