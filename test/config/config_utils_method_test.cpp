#include "config_parser.hpp"
#include "server_info.hpp"
#include "gtest/gtest.h"
#include "config_utils.hpp"


namespace {
    // // 1's server block
    // TEST(ser, outputtest) {
    //     // required variables
    //     std::vector<ServerInfo> server_blocks;
    //     ConfigParser::server_configs_type use_map;

    //     // setting value
    //     std::string config("./conf/example.conf");
    //     // std::string test_uri("/index.html");
    //     ServerInfo outp;

    //     // config parsing
    //     ConfigParser config_parser(config.c_str());
    //     config_parser.Parse(server_blocks);
    //     config_parser.ParseConfigs(use_map, server_blocks);

    //     // test: (uri: `/`) 1번째 host: 1번째 server_info: location 없을 경우
    //     outp = FindServerInfoToRequestHost("localhost", use_map.find("0.0.0.0:4343")->second);
    //     EXPECT_EQ(outp.GetClientMaxBodySize(), server_blocks[0].GetClientMaxBodySize());
    // }
    TEST(ser, outputtest2) {
        // required variables
        std::vector<ServerInfo> server_blocks;
        ConfigParser::server_configs_type use_map;

        // setting value
        std::string config("./conf/sample.conf");
        // std::string test_uri("/index.html");
        ServerInfo outp;

        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(use_map, server_blocks);

        // test: (uri: `/`) 1번째 host: 1번째 server_info: location 없을 경우
        outp = *FindServerInfoToRequestHost("localhost", use_map.find("0.0.0.0:8282")->second);
        // EXPECT_EQ(outp.GetClientMaxBodySize(), server_blocks[0].GetClientMaxBodySize());
        int test = FindLocationInfoToUri("/a/b/c/d", outp);
        EXPECT_EQ(0, test);
    }
    // TEST(config_test, redirect_test) {
    //     // required variables
    //     std::vector<ServerInfo> server_blocks;
    //     ConfigParser::server_configs_type use_map;

    //     // setting value
    //     std::string config("./conf/example.conf");
    //     // config parsing
    //     ConfigParser config_parser(config.c_str());
    //     config_parser.Parse(server_blocks);
    //     config_parser.ParseConfigs(use_map, server_blocks);
    //     EXPECT_EQ("/test/index.html", server_blocks[0].GetLocations()[0].GetRedirect());
    //     // test: (uri: `/`) 1번째 host: 1번째 server_info: location 없을 경우

    // }
    // TEST(config_test, cgi_in_serverinfo) {
    //     // required variables
    //     std::vector<ServerInfo> server_blocks;
    //     ConfigParser::server_configs_type use_map;

    //     // setting value
    //     std::string config("./conf/example.conf");
    //     // config parsing
    //     ConfigParser config_parser(config.c_str());
    //     config_parser.Parse(server_blocks);
    //     config_parser.ParseConfigs(use_map, server_blocks);

    //     server_blocks[0].SetLocationIndex(0);
    //     // std::cout << "TEST : " << server_blocks[0].GetLocationIndex() << std::endl;
    //     // EXPECT_EQ(-1,server_blocks[0].GetLocationIndex());
    //     // EXPECT_EQ("", server_blocks[0].GetPath());
    //     // EXPECT_EQ(false, server_blocks[0].IsAutoIndex());
    //     std::cout << server_blocks[0].IsRedirect() << std::endl;
    //     EXPECT_EQ(true, server_blocks[0].IsRedirect());
    //     // test: (uri: `/`) 1번째 host: 1번째 server_info: location 없을 경우

    // }
}
