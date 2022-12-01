//
// Created by Alvin Lee on 2022/12/01.
//
#include "get.hpp"
#include "gtest/gtest.h"
#include <iostream>

TEST(GetTest, StatusCode) {
    std::string body_entity;
    int status_code = GetMethod("/Users/alvinlee/Git_Folder/42/webserv/docs/index.html", &body_entity);
    EXPECT_EQ(status_code, 200);
}

TEST(GetTest, BodyTest) {
    std::string body_entity;
    int status_code = GetMethod("/Users/alvinlee/Git_Folder/42/webserv/docs/index.html", &body_entity);
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
                                       "</html>"
    EXPECT_EQ(body_entity, body_entity_expected);
}