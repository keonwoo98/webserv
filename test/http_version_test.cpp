#include <gtest/gtest.h>

#include "message/http_version.hpp"

TEST(VersionTest, Normal) {
	HttpVersion version(1, 1);
	std::string actual = version.toString();
	std::string expected = "HTTP/1.1";
	EXPECT_EQ(actual, expected);
}