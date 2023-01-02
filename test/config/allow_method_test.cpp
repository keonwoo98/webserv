#include "config_parser.hpp"
#include "server_info.hpp"
#include "gtest/gtest.h"
#include "config_utils.hpp"
#include "allow_methods.hpp"
#include <iostream>

namespace {
    TEST(config_test, allow_method) {
        // required variables
        std::vector<ServerInfo> server_blocks;
        ConfigParser::server_configs_type use_map;

        // setting value
        std::string config("./conf/example.conf");
        // config parsing
        ConfigParser config_parser(config.c_str());
        config_parser.Parse(server_blocks);
        config_parser.ParseConfigs(use_map, server_blocks);

        server_blocks[0].SetLocationIndex(0);
        // std::cout << "TEST : " << server_blocks[0].GetLocationIndex() << std::endl;
        // EXPECT_EQ(-1,server_blocks[0].GetLocationIndex());
        // EXPECT_EQ("", server_blocks[0].GetPath());
        // EXPECT_EQ(false, server_blocks[0].IsAutoIndex());
        // std::cout << server_blocks[0].IsRedirect() << std::endl;
        std::cout << server_blocks[0].GetLocations()[0] << std::endl;
        EXPECT_EQ(false, server_blocks[0].IsAllowedMethod("DELETE"));
        // test: (uri: `/`) 1번째 host: 1번째 server_info: location 없을 경우

    }
}