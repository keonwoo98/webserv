//
// Created by Alvin Lee on 2022/12/23.
//

#include "resolve_uri.hpp"
#include "config_parser.hpp"
#include "server_info.hpp"
#include "gtest/gtest.h"

namespace {
    // root 에 readable한 index.html, index2.html파일 있는경우
    // root ./www/s_index_test_1 사용
    TEST(ResolveURI, IndexTestWithURI1) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = -1; // SETTING NEED
        std::string test_uri("/"); // SETTING NEED

        /* expected value */
        std::string expected_resolved_uri("./www/s_index_test_1/index.html");
        bool expected_auto_index = false;
        bool expected_cgi = false;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        Resolve_URI(client, &user_data);
        EXPECT_EQ(user_data.request_message_.GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(user_data.request_message_.GetIsAutoIndex(), expected_auto_index);
        EXPECT_EQ(user_data.request_message_.GetIsCgi(), expected_cgi);
    }

    // root ./www/s_index_test_2 사용
    TEST(ResolveURI, IndexTestWithURI2) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = -1; // SETTING NEED
        std::string test_uri("/"); // SETTING NEED

        /* expected value */
        std::string expected_resolved_uri("./www/s_index_test_2/index2.html");
        bool expected_auto_index = false;
        bool expected_cgi = false;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        Resolve_URI(client, &user_data);
        EXPECT_EQ(user_data.request_message_.GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(user_data.request_message_.GetIsAutoIndex(), expected_auto_index);
        EXPECT_EQ(user_data.request_message_.GetIsCgi(), expected_cgi);
    }

    // root ./www/s_index_test_3 사용
    TEST(ResolveURI, IndexTestWithURI3) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = -1; // SETTING NEED
        std::string test_uri("/"); // SETTING NEED

        /* expected value */
        std::string expected_resolved_uri("./www/s_index_test_3/index.html");
        bool expected_auto_index = false;
        bool expected_cgi = false;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        Resolve_URI(client, &user_data);
        EXPECT_EQ(user_data.request_message_.GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(user_data.request_message_.GetIsAutoIndex(), expected_auto_index);
        EXPECT_EQ(user_data.request_message_.GetIsCgi(), expected_cgi);
    }

    // root 에 파일이 존재하지 않는경우 + auto index on
    // root ./www/e_empty 사용
    TEST(ResolveURI, IndexTestWithURI4) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = -1; // SETTING NEED
        std::string test_uri("/"); // SETTING NEED

        /* expected value */
        bool expected_auto_index = true;
        bool expected_cgi = false;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        Resolve_URI(client, &user_data);
        EXPECT_EQ(user_data.request_message_.GetIsAutoIndex(), expected_auto_index);
        EXPECT_EQ(user_data.request_message_.GetIsCgi(), expected_cgi);
    }

    // root 에 파일이 권한이 없는 경우 + auto index on
    // root ./www/e_nopermit_1 사용 (chmod 000 index.html)
    TEST(ResolveURI, IndexTestWithURI5) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = -1; // SETTING NEED
        std::string test_uri("/"); // SETTING NEED

        /* expected value */
        bool expected_auto_index = true;
        bool expected_cgi = false;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        EXPECT_THROW({
                         try {
                             Resolve_URI(client, &user_data);
                         } catch (const HttpException &e) {
                             EXPECT_STREQ("has no permission\n", e.what());
                             throw;
                         }
                     }, HttpException
        );
    }
    // root 에 파일이 권한이 없는 경우 + auto index on
    // root ./www/e_nopermit_2 사용 (chmod 000 index2.html)
    TEST(ResolveURI, IndexTestWithURI6) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = -1; // SETTING NEED
        std::string test_uri("/"); // SETTING NEED

        /* expected value */
        bool expected_auto_index = true;
        bool expected_cgi = false;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        EXPECT_THROW({
                         try {
                             Resolve_URI(client, &user_data);
                         } catch (const HttpException &e) {
                             EXPECT_STREQ("has no permission\n", e.what());
                             throw;
                         }
                     }, HttpException
        );
    }
    // root 에 readable한 index2.html 직접 가져오기
    // root ./www/s_index_test_1 사용
    TEST(ResolveURI, URITEST1) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = -1; // SETTING NEED
        std::string test_uri("/index2.html"); // SETTING NEED

        /* expected value */
        std::string expected_resolved_uri("./www/s_index_test_1/index2.html");
        bool expected_auto_index = false;
        bool expected_cgi = false;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        Resolve_URI(client, &user_data);
        EXPECT_EQ(user_data.request_message_.GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(user_data.request_message_.GetIsAutoIndex(), expected_auto_index);
        EXPECT_EQ(user_data.request_message_.GetIsCgi(), expected_cgi);
    }
    // root 에 readable한 없는 파일 가져오기
    // root ./www/s_index_test_1 사용
    TEST(ResolveURI, URITEST2) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = -1; // SETTING NEED
        std::string test_uri("/index3.html"); // SETTING NEED

        /* expected value */
        bool expected_auto_index = false;
        bool expected_cgi = false;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        EXPECT_THROW({
                         try {
                             Resolve_URI(client, &user_data);
                         } catch (const HttpException &e) {
                             EXPECT_STREQ("file not exist\n", e.what());
                             throw;
                         }
                     }, HttpException
        );
    }
    // root 에 permision 없는 파일 가져오기
    // root ./www/e_nopermit_1 사용
    TEST(ResolveURI, URITEST3) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = -1; // SETTING NEED
        std::string test_uri("/index.html"); // SETTING NEED

        /* expected value */
        bool expected_auto_index = false;
        bool expected_cgi = false;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        EXPECT_THROW({
                         try {
                             Resolve_URI(client, &user_data);
                         } catch (const HttpException &e) {
                             EXPECT_STREQ("has no permission\n", e.what());
                             throw;
                         }
                     }, HttpException
        );
    }
    // root 에 cgi 존재
    // location /board/content
    // root ./www/html/contents;
    TEST(ResolveURI, CGITEST1) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = 1; // SETTING NEED
        std::string test_uri("/board/content/test_cgi.php"); // SETTING NEED

        /* expected value */
        std::string expected_resolved_uri("./www/html/contents/board/content/test_cgi.php");
        std::string expected_query_string("");
        std::string expected_cgi_exe_path("php-cgi");
        bool expected_auto_index = false;
        bool expected_cgi = true;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        Resolve_URI(client, &user_data);

        EXPECT_EQ(user_data.request_message_.GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(user_data.request_message_.GetCgiQuery(), expected_query_string);
        EXPECT_EQ(user_data.request_message_.GetCgiExePath(), expected_cgi_exe_path);
        EXPECT_EQ(user_data.request_message_.GetIsAutoIndex(), expected_auto_index);
        EXPECT_EQ(user_data.request_message_.GetIsCgi(), expected_cgi);
    }
    // 1 + uery string 까지 받기
    TEST(ResolveURI, CGITEST2) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = 1; // SETTING NEED
        std::string test_uri("/board/content/test_cgi.php?sdfsdf=sdfsdf&sdfsd=sdfsdf"); // SETTING NEED

        /* expected value */
        std::string expected_resolved_uri("./www/html/contents/board/content/test_cgi.php");
        std::string expected_query_string("sdfsdf=sdfsdf&sdfsd=sdfsdf");
        std::string expected_cgi_exe_path("php-cgi");
        bool expected_auto_index = false;
        bool expected_cgi = true;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);
        Resolve_URI(client, &user_data);

        EXPECT_EQ(user_data.request_message_.GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(user_data.request_message_.GetCgiQuery(), expected_query_string);
        EXPECT_EQ(user_data.request_message_.GetCgiExePath(), expected_cgi_exe_path);
        EXPECT_EQ(user_data.request_message_.GetIsAutoIndex(), expected_auto_index);
        EXPECT_EQ(user_data.request_message_.GetIsCgi(), expected_cgi);
    }
    // 잘못된 파일 이름
    TEST(ResolveURI, CGITEST3) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = 1; // SETTING NEED
        std::string test_uri("/board/content/test_cgi.ph"); // SETTING NEED

        /* expected value */
        std::string expected_resolved_uri("./www/html/contents/board/content/test_cgi.php");
        std::string expected_query_string("sdfsdf=sdfsdf&sdfsd=sdfsdf");
        std::string expected_cgi_exe_path("php-cgi");
        bool expected_auto_index = false;
        bool expected_cgi = true;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        EXPECT_THROW({
                         try {
                             Resolve_URI(client, &user_data);
                         } catch (const HttpException &e) {
                             EXPECT_STREQ("file not exist\n", e.what());
                             throw;
                         }
                     }, HttpException
        );
    }
    // location 에 index 존재할때
    // board.html을 ./www/html/contents/board/content/ 에 생
    TEST(ResolveURI, CGITEST4) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = 1; // SETTING NEED
        std::string test_uri(
                "/board/content"); // SETTING NEED 마지막 slash 없이 들어온다는 가정 -> request_message 에 저장할때 그렇게 주면 좋을듯

        /* expected value */
        std::string expected_resolved_uri("./www/html/contents/board/content/board.html");
        bool expected_auto_index = false;
        bool expected_cgi = false;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        Resolve_URI(client, &user_data);
        EXPECT_EQ(user_data.request_message_.GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(user_data.request_message_.GetIsAutoIndex(), expected_auto_index);
        EXPECT_EQ(user_data.request_message_.GetIsCgi(), expected_cgi);
    }
    // location 에 index 존재 x
    // ./www/html/contents/board/content/ 에 index file x
    TEST(ResolveURI, CGITEST5) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = 1; // SETTING NEED
        std::string test_uri(
                "/board/content"); // SETTING NEED 마지막 slash 없이 들어온다는 가정 -> request_message 에 저장할때 그렇게 주면 좋을듯

        /* expected value */
        std::string expected_resolved_uri("./www/html/contents/board/content/board.html");
        bool expected_auto_index = true;
        bool expected_cgi = false;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        Resolve_URI(client, &user_data);
        EXPECT_EQ(user_data.request_message_.GetIsAutoIndex(), expected_auto_index);
        EXPECT_EQ(user_data.request_message_.GetIsCgi(), expected_cgi);
    }
// location 에 index 존재 x
    // ./www/html/contents/board/content/ 에 index file x
    TEST(ResolveURI, LOACTIONTEST) {
        // setting value
        std::string config("./conf/example.conf"); // SETTING NEED
        int location_idx = 1; // SETTING NEED
        std::string test_uri(
                "/board/content/test.html"); // SETTING NEED 마지막 slash 없이 들어온다는 가정 -> request_message 에 저장할때 그렇게 주면 좋을듯

        /* expected value */
        std::string expected_resolved_uri("./www/html/contents/board/content/test.html");
        bool expected_auto_index = false;
        bool expected_cgi = false;


        // required variables
        std::vector<ServerInfo> server_blocks;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(server_configs, server_blocks);

        // make dummy data
        ClientSocket client(0, server_blocks, clientaddr);
        client.setServerInfoIt(server_blocks.begin()); // 1's server block (SETTING NEED)
        client.setLocationIndex(location_idx);
        Udata user_data(Udata::RECV_REQUEST, 0);
        user_data.request_message_.setUri(test_uri);

        Resolve_URI(client, &user_data);
        EXPECT_EQ(user_data.request_message_.GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(user_data.request_message_.GetIsAutoIndex(), expected_auto_index);
        EXPECT_EQ(user_data.request_message_.GetIsCgi(), expected_cgi);
    }
}
