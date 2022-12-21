//
// Created by 김민준 on 2022/12/20.
//

#include "header.h"
#include "gtest/gtest.h"
#include "character_const.hpp"

#include <sstream>

TEST(response_header_test, basic_header_test) {
	Header response_header;
	response_header.AddServer();

	std::stringstream ss;
	ss << response_header;

	std::string expected = "Server: Webserv" DCRLF;
	EXPECT_EQ(expected, ss.str());
}

TEST(response_header_test, header_date_test) {
	Header response_header;
	response_header.AddDate(std::time(NULL));

	std::stringstream ss;
	ss << response_header;

	std::cerr << ss.str() << std::endl;
}

TEST(response_header_test, allow_test) {
	Header response_header;
	std::vector<std::string> allow_method = {"GET", "POST", "DELETE"};

	response_header.AddAllow(allow_method);

	std::stringstream ss;
	ss << response_header;

	std::string expected = "Allow: GET, POST, DELETE" DCRLF;
	EXPECT_EQ(expected, ss.str());
}

TEST(response_header_test, content_type_test) {
	Header response_header;
	response_header.AddContentType("text/html");

	std::stringstream ss;
	ss << response_header;

	std::string expected = "Content-Type: text/html" DCRLF;
	EXPECT_EQ(expected, ss.str());
}

TEST(response_header_test, location_test) {
	Header response_header;
	response_header.AddLocation("/index.html");

	std::stringstream ss;
	ss << response_header;

	std::string expected = "Location: /index.html" DCRLF;
	EXPECT_EQ(expected, ss.str());
}

TEST(response_header_test, connection_test) {
	Header response_header;
	response_header.AddConnection("keep-alive");

	std::stringstream ss;
	ss << response_header;

	std::string expected = "Connection: keep-alive" DCRLF;
	EXPECT_EQ(expected, ss.str());
}

TEST(response_header_test, transfer_encoding_test) {
	Header response_header;
	response_header.AddTransferEncoding("chunked");

	std::stringstream ss;
	ss << response_header;

	std::string expected = "Transfer-Encoding: chunked" DCRLF;
	EXPECT_EQ(expected, ss.str());
}

TEST(response_header_test, multiple_header_test) {
	Header response_header;
	std::string body = "Hello World!"; // 12
	response_header.AddServer();
	response_header.AddContentType("text/plain");
	response_header.AddConnection("keep-alive");
	response_header.AddContentLength(body);

	std::stringstream ss;
	ss << response_header;


	std::string expected = "Connection: keep-alive" CRLF
						   "Content-Length: 12" CRLF
						   "Content-Type: text/plain" CRLF
						   "Server: Webserv" DCRLF;
	EXPECT_EQ(expected, ss.str());
}
