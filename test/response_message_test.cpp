#include "message/response_message.hpp"
#include "status_code.hpp"
#include "gtest/gtest.h"

TEST(RESPONSE_MESSAGE_TEST, ToStringTest) {
	HttpVersion version(1, 1);
	StatusLine status_line(version, OK, "OK");

	Header header;
	std::vector<std::string> value;
	value.push_back("128");
	std::string key = "Content-Length";
	header.Add(key, value);

	Body body("<html>\n</html>");

	ResponseMessage response_message(status_line, header, body);
	std::string actual = response_message.toString();
	std::string expected = "HTTP/1.1 200 OK\r\n"
						   "Content-Length: 128\r\n\r\n"
						   "<html>\n"
						   "</html>";
	EXPECT_EQ(actual, expected);
}
