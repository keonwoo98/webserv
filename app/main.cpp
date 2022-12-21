#include <iostream>

#include "config_parser.hpp"
#include "webserv.hpp"

int main(int argc, char **argv) {
	std::string config;
	// Webserv webserv;
	std::vector<ServerInfo> server_blocks;

	if (argc > 2) {
		std::cerr << "Usage: ./webserv [config]" << std::endl;
		return EXIT_FAILURE;
	}

	if (argc == 1)
		config = "./conf/default.config";
	else
		config = argv[1];
	try {
		ConfigParser config_parser(config.c_str());
		config_parser.Parse(server_blocks);
		// config_parser.PrintConf(server_blocks);
		for (size_t i = 0; i < server_blocks.size(); i++)
			std::cout << server_blocks[i] << std::endl;

		// webserv.SetupServer(server_blocks);
		// webserv.StartServer();
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
	}
	return 0;
}
