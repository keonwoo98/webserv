//
// Created by 김민준 on 2022/12/20.
//

#include "gtest/gtest.h"
#include "response_message.hpp"
#include "status_code.hpp"
#include "character_const.hpp"

namespace {

TEST(response_message_test, basic_response) {
	ResponseMessage response_message(OK, "OK");
	std::string body = "Hello World!";
	response_message.SetBody(body);

	std::stringstream ss;
	ss << response_message;

	std::string expected = "HTTP/1.1 200 OK" CRLF
						   "Content-Length: 12" CRLF
						   "Server: Webserv" DCRLF
						   "Hello World!";
	EXPECT_EQ(expected, ss.str());
}
}
