//
// Created by Alvin Lee on 2022/12/13.
//

#include "post_method.hpp"
#include "gtest/gtest.h"
#include <iostream>

namespace {
TEST(post_test, status_code_200) {
	std::string body_entity;
	body_entity = "";
	int status_code = PostMethod("./docs/index.html", body_entity);
	EXPECT_EQ(status_code, 200);
}

TEST(post_test, body_test) {
	std::string body_entity = "";
	int status_code = PostMethod("./docs/index.html", body_entity);
	std::string body_entity_expected = "<!DOCTYPE html>\n"
									   "<html>\n"
									   "<head>\n"
									   "<title>Welcome to Webserv</title>\n"
									   "<style>\n"
									   "html { color-scheme: white; }\n"
									   "body { width: 35em; margin: 0 auto;\n"
									   "\t\tfont-family: Tahoma, Verdana, Arial, sans-serif; }\n"
									   "</style>\n"
									   "</head>\n"
									   "<body>\n"
									   "<h1>Welcome to Webserv!</h1>\n"
									   "<p>If you see this message. Webserv project is working!</p>\n"
									   "<p><em>Thank you for your evaluation.</em></p>\n"
									   "</body>\n"
									   "</html>\n";
	EXPECT_EQ(body_entity, body_entity_expected);
}

TEST(post_test, status_code_404) {
	std::string body_entity;
	body_entity = "";
	int status_code = PostMethod("./docs/new.html", body_entity);
	EXPECT_EQ(status_code, 404);
}

//TEST(PostCGITest, status_code_200) {
//	std::string body_entity;
//	body_entity = "";
//	int status_code = PostMethod("./docs/cgi-bin/index.html", body_entity);
//	EXPECT_EQ(status_code, 200);
//}

}
