//
// Created by Alvin Lee on 2022/12/23.
//

#include "resolve_uri.hpp"
#include "config_parser.hpp"
#include "server_info.hpp"
#include "gtest/gtest.h"

namespace {

    // server block 존재할 때
    class ServerBlockSet : public ::testing::Test {
    protected:
        int location_idx_ = -1;
        std::vector<ServerInfo> server_blocks;
        ServerInfo server_info;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;
        ClientSocket *client;
        Udata *user_data;
        ResolveURI *r_uri;

        void SetUp() override {
            // Config file Parse
            std::string config("./test/uri/conf/example.conf"); // SETTING NEED
            ConfigParser config_parser(config.c_str());
            config_parser.Parse(server_blocks);
            config_parser.ParseConfigs(server_configs, server_blocks);

            // make dummy data
            client = new ClientSocket(0, 0, clientaddr);
            server_info = server_blocks.at(0);
            client->SetServerInfo(server_info);
            client->SetLocationIndex(location_idx_);

            user_data = new Udata(Udata::RECV_REQUEST, 0);
            r_uri = new ResolveURI(server_info, user_data->request_message_);
        }

        void TearDown() override {
            delete client;
            delete user_data;
            delete r_uri;
        }
    };

    // location block 존재할 때
    class LocationBlockSet : public ::testing::Test {
    protected:
        int location_idx_ = 1;
        std::vector<ServerInfo> server_blocks;
        ServerInfo server_info;
        struct sockaddr_in clientaddr;
        ConfigParser::server_configs_type server_configs;
        ClientSocket *client;
        Udata *user_data;
        ResolveURI *r_uri;

        void SetUp() override {
            // Config file Parse
            std::string config("./test/uri/conf/example.conf"); // SETTING NEED
            ConfigParser config_parser(config.c_str());
            config_parser.Parse(server_blocks);
            config_parser.ParseConfigs(server_configs, server_blocks);

            // make dummy data
            client = new ClientSocket(0, 0, clientaddr);
            server_info = server_blocks.at(0);
            server_info.SetLocationIndex(location_idx_);
            client->SetServerInfo(server_info);
            client->SetLocationIndex(location_idx_);

            user_data = new Udata(Udata::RECV_REQUEST, 0);
            r_uri = new ResolveURI(server_info, user_data->request_message_);
        }

        void TearDown() override {
            delete client;
            delete user_data;
            delete r_uri;
        }
    };


    // root ./test/uri/test_cases/s_index_test_all_exist 사용
    TEST_F(ServerBlockSet, IndexTestAllExist) {
        /* setting value */
        std::string test_uri(""); // SETTING NEED
        /* expected value */
        std::string expected_resolved_uri("./test/uri/test_cases/s_index_test_all_exist/index.html");
        bool expected_auto_index = false;
        bool expected_cgi = false;
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        r_uri->Run();
        EXPECT_EQ(r_uri->GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(r_uri->IsAutoIndex(), expected_auto_index);
        EXPECT_EQ(r_uri->IsCgi(), expected_cgi);
    }

    // root ./test/uri/test_cases/s_index_test_second_exist 사용
    TEST_F(ServerBlockSet, IndexTestSecondExist) {
        /* setting value */
        std::string test_uri(""); // SETTING NEED
        /* expected value */
        std::string expected_resolved_uri("./test/uri/test_cases/s_index_test_second_exist/index2.html");
        bool expected_auto_index = false;
        bool expected_cgi = false;
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        r_uri->Run();
        EXPECT_EQ(r_uri->GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(r_uri->IsAutoIndex(), expected_auto_index);
        EXPECT_EQ(r_uri->IsCgi(), expected_cgi);
    }

    // root ./test/uri/test_cases/s_index_test_first_exist 사용
    TEST_F(ServerBlockSet, IndexTestFirstExist) {
        /* setting value */
        std::string test_uri(""); // SETTING NEED
        /* expected value */
        std::string expected_resolved_uri("./test/uri/test_cases/s_index_test_first_exist/index.html");
        bool expected_auto_index = false;
        bool expected_cgi = false;
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        r_uri->Run();
        EXPECT_EQ(r_uri->GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(r_uri->IsAutoIndex(), expected_auto_index);
        EXPECT_EQ(r_uri->IsCgi(), expected_cgi);
    }

    // root ./test/uri/test_cases/e_empty 사용
    // auto index => true
    TEST_F(ServerBlockSet, IndexTestEmpty) {
        /* setting value */
        std::string test_uri("/"); // SETTING NEED
        /* expected value */
        bool expected_auto_index = true;
        bool expected_cgi = false;
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        r_uri->Run();
        EXPECT_EQ(r_uri->IsAutoIndex(), expected_auto_index);
        EXPECT_EQ(r_uri->IsCgi(), expected_cgi);
    }

    // root ./test/uri/test_cases/e_no_permit_first_one 사용 (chmod 000 index.html)
    TEST_F(ServerBlockSet, IndexTestNoPermitFirstOne) {
        /* setting value */
        std::string test_uri("/"); // SETTING NEED
        /* expected value */
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        EXPECT_THROW({
                         try {
                             r_uri->Run();
                         } catch (const HttpException &e) {
                             EXPECT_STREQ("has no permision\n", e.what());
                             throw;
                         }
                     }, HttpException
        );
    }
    // root ./test/uri/test_cases/e_no_permit_second_one 사용 (chmod 000 index2.html)
    TEST_F(ServerBlockSet, IndexTestNoPermitSecondOne) {
        /* setting value */
        std::string test_uri("/"); // SETTING NEED
        /* expected value */
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        EXPECT_THROW({
                         try {
                             r_uri->Run();
                         } catch (const HttpException &e) {
                             EXPECT_STREQ("has no permision\n", e.what());
                             throw;
                         }
                     }, HttpException
        );
    }

    // root ./test/uri/test_cases/s_index_test_all_exist 사용
    TEST_F(ServerBlockSet, UriTestExistFile) {
        /* setting value */
        std::string test_uri("/index2.html"); // SETTING NEED
        /* expected value */
        std::string expected_resolved_uri("./test/uri/test_cases/s_index_test_all_exist/index2.html");
        bool expected_auto_index = false;
        bool expected_cgi = false;
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        r_uri->Run();
        EXPECT_EQ(r_uri->GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(r_uri->IsAutoIndex(), expected_auto_index);
        EXPECT_EQ(r_uri->IsCgi(), expected_cgi);
    }

    // root ./test/uri/test_cases/s_index_test_all_exist 사용
    TEST_F(ServerBlockSet, UriTestNotExistFile) {
        /* setting value */
        std::string test_uri("/index3.html"); // SETTING NEED
        /* expected value */
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        EXPECT_THROW({
                         try {
                             r_uri->Run();
                         } catch (const HttpException &e) {
                             EXPECT_STREQ("file not exist\n", e.what());
                             throw;
                         }
                     }, HttpException
        );
    }

    // root ./test/uri/test_cases/s_index_test_all_exist 사용
    TEST_F(ServerBlockSet, UriTestWithNoPermit) {
        /* setting value */
        std::string test_uri("/index.html"); // SETTING NEED
        /* expected value */
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        EXPECT_THROW({
                         try {
                             r_uri->Run();
                         } catch (const HttpException &e) {
                             EXPECT_STREQ("has no permission\n", e.what());
                             throw;
                         }
                     }, HttpException
        );
    }


    // location root ./test/uri/test_cases/html/contents;
    TEST_F(LocationBlockSet, CgiTestPhpFileExist) {
        /* setting value */
        std::string test_uri("/board/content/test_cgi.php"); // SETTING NEED
        /* expected value */
        std::string expected_resolved_uri("./test/uri/test_cases/html/contents/board/content/test_cgi.php");
        std::string expected_query_string("");
        std::string expected_cgi_exe_path("php-cgi");
        bool expected_auto_index = false;
        bool expected_cgi = true;
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        r_uri->Run();
        EXPECT_EQ(r_uri->GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(r_uri->GetCgiQuery(), expected_query_string);
        EXPECT_EQ(r_uri->GetCgiPath(), expected_cgi_exe_path);
        EXPECT_EQ(r_uri->IsAutoIndex(), expected_auto_index);
        EXPECT_EQ(r_uri->IsCgi(), expected_cgi);
    }

    // location root ./test/uri/test_cases/html/contents;
    TEST_F(LocationBlockSet, CgiTestWithQueryString) {
        /* setting value */
        std::string test_uri("/board/content/test_cgi.php?sdfsdf=sdfsdf&sdfsd=sdfsdf"); // SETTING NEED
        /* expected value */
        std::string expected_resolved_uri("./test/uri/test_cases/html/contents/board/content/test_cgi.php");
        std::string expected_query_string("sdfsdf=sdfsdf&sdfsd=sdfsdf");
        std::string expected_cgi_exe_path("php-cgi");
        bool expected_auto_index = false;
        bool expected_cgi = true;
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        r_uri->Run();
        EXPECT_EQ(r_uri->GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(r_uri->GetCgiQuery(), expected_query_string);
        EXPECT_EQ(r_uri->GetCgiPath(), expected_cgi_exe_path);
        EXPECT_EQ(r_uri->IsAutoIndex(), expected_auto_index);
        EXPECT_EQ(r_uri->IsCgi(), expected_cgi);
    }

    // location root ./test/uri/test_cases/html/contents;
    TEST_F(LocationBlockSet, CgiTestPhpFileNotExist) {
        /* setting value */
        std::string test_uri("/board/content/test_cgi.ph"); // SETTING NEED
        /* expected value */
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        EXPECT_THROW({
                         try {
                             r_uri->Run();
                         } catch (const HttpException &e) {
                             EXPECT_STREQ("file not exist\n", e.what());
                             throw;
                         }
                     }, HttpException
        );
    }
    // location root ./test/uri/test_cases/html/contents
    // board.html 생성하고 test
    TEST_F(LocationBlockSet, IndexTestExist) {
        /* setting value */
        std::string test_uri("/board/content"); // SETTING NEED
        /* expected value */
        std::string expected_resolved_uri("./test/uri/test_cases/html/contents/board/content/board.html");
        bool expected_auto_index = false;
        bool expected_cgi = false;
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        r_uri->Run();
        EXPECT_EQ(r_uri->GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(r_uri->IsAutoIndex(), expected_auto_index);
        EXPECT_EQ(r_uri->IsCgi(), expected_cgi);
    }

    // location root ./test/uri/test_cases/html/contents
    // board.html 삭제하고 test
    TEST_F(LocationBlockSet, IndexTestNotExist) {
        /* setting value */
        std::string test_uri("/board/content"); // SETTING NEED
        /* expected value */
        bool expected_auto_index = true;
        bool expected_cgi = false;
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        r_uri->Run();
        EXPECT_EQ(r_uri->IsAutoIndex(), expected_auto_index);
        EXPECT_EQ(r_uri->IsCgi(), expected_cgi);
    }

    // location root ./test/uri/test_cases/html/contents
    TEST_F(LocationBlockSet, LocationTest) {
        /* setting value */
        std::string test_uri("/board/content/test.html"); // SETTING NEED
        /* expected value */
        std::string expected_resolved_uri("./test/uri/test_cases/html/contents/board/content/test.html");
        bool expected_auto_index = false;
        bool expected_cgi = false;
        /* add dummy data */
        user_data->request_message_.setUri(test_uri);

        r_uri->Run();
        EXPECT_EQ(r_uri->GetResolvedUri(), expected_resolved_uri);
        EXPECT_EQ(r_uri->IsAutoIndex(), expected_auto_index);
        EXPECT_EQ(r_uri->IsCgi(), expected_cgi);
    }
}
