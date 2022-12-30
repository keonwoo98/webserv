//
// Created by 김민준 on 2022/12/20.
//

#include "gtest/gtest.h"
#include "response_message.hpp"
#include "status_code.hpp"
#include "character_const.hpp"

namespace {

TEST(build_response_mssage, basic_message) {
	ResponseMessage response_message(OK, "OK");
	std::string body = "Hello World!";
	response_message.AppendBody(body.c_str());

	std::stringstream ss;
	ss << response_message;

	std::string expected = "HTTP/1.1 200 OK" CRLF
						   "Content-Length: 12" CRLF
						   "Server: Webserv" DCRLF
						   "Hello World!";
	EXPECT_EQ(expected, ss.str());
}

TEST(build_response_message, append_body) {
	ResponseMessage response_message(NOT_FOUND, "NOT_FOUND");
	std::string body = "Not Found\n"; // 10
	response_message.AppendBody(body.c_str());

	std::string append_body = "can't find uri"; // 14
	response_message.AppendBody(append_body.c_str());

	std::stringstream ss;
	ss << response_message;

	std::string expected = "HTTP/1.1 404 NOT_FOUND" CRLF
						   "Content-Length: 24" CRLF
						   "Server: Webserv" DCRLF
						   "Not Found\n"
						   "can't find uri";
	EXPECT_EQ(expected, ss.str());
}
}
