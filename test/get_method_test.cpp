//
// Created by Alvin Lee on 2022/12/01.
//
#include "get_method.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <fstream>

namespace {
TEST(get_test, statis_code_200) {
	std::string body_entity;
	int status_code = GetMethod("./docs/index.html", body_entity);
	EXPECT_EQ(status_code, 200);
}

TEST(get_test, body_test) {
	std::string body_entity;
	int status_code = GetMethod("./docs/index.html", body_entity);
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

TEST(get_test, status_code_404) {
	std::string body_entity;
	int status_code = GetMethod("./docs/index", body_entity);
	EXPECT_EQ(status_code, 404);
}

TEST(get_test, status_code_403) {
	std::string body_entity;
	std::ofstream ofs("./docs/403_forbidden.html"); // create file
	ofs << "403 Forbidden";
	ofs.close();

	system("chmod 000 ./docs/403_forbidden.html");

	int status_code = GetMethod("./docs/403_forbidden.html", body_entity);
	EXPECT_EQ(status_code, 403);
	unlink("./docs/403_forbidden.html");
}
}
