#include <iostream>

#include "config_parser.hpp"
#include "webserv.hpp"
#include "config_utils.hpp"

int main(int argc, char **argv) {
	if (argc > 2) {
		std::cerr << "Usage: ./webserv [config]" << std::endl;
		return EXIT_FAILURE;
	}

	const char *config;
	if (argc == 1)
		config = "./conf/test1.conf";
	else
		config = argv[1];
	std::vector<ServerInfo> server_blocks;
	ConfigParser::server_configs_type server_configs;
	try {
		ConfigParser config_parser(config);
		config_parser.Parse(server_blocks);
		config_parser.PrintConf(server_blocks);
		config_parser.ParseConfigs(server_configs, server_blocks);
		Webserv webserv(server_configs);
		webserv.RunServer();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
