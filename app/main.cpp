#include <iostream>
#include "config_parser.hpp"
#include "webserv.hpp"

int main(int argc, char **argv) {
	std::string config;
	Webserv webserv;
	std::vector<ServerInfo> server_blocks;
	typedef std::map<std::string, std::vector<ServerInfo> > use_type;
	use_type use_map;

	if (argc > 2) { 
		std::cerr << "Usage: ./webserv [config]" << std::endl;
		return EXIT_FAILURE;
	}

	if (argc == 1)
		config = "./conf/example.conf";
	else
		config = argv[1];
	try {
		ConfigParser config_parser(config.c_str());
		config_parser.Parse(server_blocks);
		config_parser.PrintConf(server_blocks);
		config_parser.ParseUse(use_map, server_blocks);
		webserv.SetupServer(server_blocks);
		webserv.StartServer();

	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
