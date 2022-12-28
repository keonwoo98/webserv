//
// Created by Alvin Lee on 2022/12/23.
//

#include "resolve_uri.hpp"
#include "config_parser.hpp"
#include "server_info.hpp"
#include "gtest/gtest.h"

namespace {
    // 1's server block
    TEST(ResolveURI, IndexTestWithURI1) {
        // required variables
        std::vector<ServerInfo> server_blocks;
        ConfigParser::use_type use_map;

        // setting value
        std::string config("./conf/example.conf");
        std::string test_uri("/index.html");
        std::vector<std::string> test_resolved_uri;

        std::vector<std::string> expected_resolved_uri;
        /* expected value */
        expected_resolved_uri.push_back("./www/html/index.html");


        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseUse(use_map, server_blocks);

        // test: (uri: `/`) 1번째 host: 1번째 server_info: location 없을 경우
        test_resolved_uri = Resolve_URI(test_uri, server_blocks.at(0), -1);
        EXPECT_EQ(test_resolved_uri, expected_resolved_uri);
    }

    // 1's server block
    TEST(ResolveURI, IndexTestWithURI2) {
        // required variables
        std::vector<ServerInfo> server_blocks;
        ConfigParser::use_type use_map;

        // setting value
        std::string config("./conf/example.conf");
        std::string test_uri("/board/content/index.html");
        std::vector<std::string> test_resolved_uri;

        std::vector<std::string> expected_resolved_uri;
        /* expected value */
        expected_resolved_uri.push_back("./www/html/contents/board/content/index.html");

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseUse(use_map, server_blocks);

        // test: (uri: `/`) 1번째 host: 1번째 server_info: location 없을 경우
        test_resolved_uri = Resolve_URI(test_uri, server_blocks.at(0), 1);
        EXPECT_EQ(test_resolved_uri, expected_resolved_uri);
    }

    TEST(ResolveURI, IndexTestWithOutURI1) {
        // required variables
        std::vector<ServerInfo> server_blocks;
        ConfigParser::use_type use_map;

        // setting value
        std::string config("./conf/example.conf");
        std::string test_uri("/");
        std::vector<std::string> test_resolved_uri;

        std::vector<std::string> expected_resolved_uri;
        /* expected value */
        expected_resolved_uri.push_back("./www/html/index.html");
        expected_resolved_uri.push_back("./www/html/index2.html");

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseUse(use_map, server_blocks);

        // test: (uri: `/`) 1번째 host: 1번째 server_info: location 없을 경우
        test_resolved_uri = Resolve_URI(test_uri, server_blocks.at(0), -1);
        EXPECT_EQ(test_resolved_uri, expected_resolved_uri);
    }

    TEST(ResolveURI, IndexTestWithOutURI2) {
        // required variables
        std::vector<ServerInfo> server_blocks;
        ConfigParser::use_type use_map;

        // setting value
        std::string config("./conf/example.conf");
        std::string test_uri("/board/content");
        std::vector<std::string> test_resolved_uri;

        std::vector<std::string> expected_resolved_uri;
        /* expected value */
        expected_resolved_uri.push_back("./www/html/contents/board/content/board.html");

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseUse(use_map, server_blocks);

        // test: (uri: `/`) 1번째 host: 1번째 server_info: location 없을 경우
        test_resolved_uri = Resolve_URI(test_uri, server_blocks.at(0), 1);
        EXPECT_EQ(test_resolved_uri, expected_resolved_uri);
    }
}