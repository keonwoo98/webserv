//
// Created by 김민준 on 2022/12/19.
//

#include "gtest/gtest.h"
#include "http_version.h"
#include "status_line.h"
#include "status_code.hpp"

TEST(response_test, status_line_print_test) {
	StatusLine status_line(HttpVersion(1, 1), OK, "OK");

	std::stringstream ss;
	ss << status_line;

	EXPECT_EQ(ss.str(), "HTTP/1.1 200 OK\r\n");
}

TEST(response_test, status_line_no_parameter) {
	StatusLine status_line(HttpVersion(), NOT_FOUND, "NOT_FOUND");

	std::stringstream ss;
	ss << status_line;

	EXPECT_EQ(ss.str(), "HTTP/1.1 404 NOT_FOUND\r\n");
}