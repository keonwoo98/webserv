//
// Created by Alvin Lee on 2022/12/13.
//

#include "post_method.hpp"
#include "gtest/gtest.h"
#include <iostream>


TEST(PostTest, StatusCode200) {
    std::string body_entity;
    body_entity = "";
    int status_code = PostMethod("/index.html", body_entity);
    EXPECT_EQ(status_code, 200);
}

TEST(PostTest, BodyTest) {
    std::string body_entity;
    int status_code = PostMethod("/index.html", body_entity);
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

TEST(PostTest, StatusCode404) {
    std::string body_entity;
    body_entity = "";
    int status_code = PostMethod("/new1.html", body_entity);
    EXPECT_EQ(status_code, 404);
}

TEST(PostCGITest, StatusCode200) {
    std::string body_entity;
    body_entity = "";
    int status_code = PostMethod("/cgi/index.html", body_entity);
    EXPECT_EQ(status_code, 200);
}