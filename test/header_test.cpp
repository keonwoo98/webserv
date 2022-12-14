#include "message/header.hpp"
#include "gtest/gtest.h"

TEST(HeaderTest, ToStringTest) {
	std::string key = "Content-Length";
	std::vector<std::string> value;
	value.push_back("128");

	Header header;
	header.Add(key, value);
	std::string expected = "Content-Length: 128\r\n\r\n";
	std::string actual = header.toString();
	EXPECT_EQ(expected, actual);
}