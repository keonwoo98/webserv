#include "gtest/gtest.h"
#include "message/status_line.hpp"

TEST(StatusLineTest, ToStringTest) {
	HttpVersion version(1, 1);
	StatusLine status_line(version, 200, "OK");
	std::string actual = status_line.toString();
	std::string expected = "HTTP/1.1 200 OK\r\n";
	EXPECT_EQ(actual, expected);
}