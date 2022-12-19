#include <iostream>

#include "config_parser.hpp"
#include "webserv.hpp"

int main(int argc, char **argv) {
	std::string config;
	Webserv webserv;

	if (argc > 2) {
		std::cerr << "Usage: ./webserv [config]" << std::endl;
		return EXIT_FAILURE;
	}

	if (argc == 1)
		config = "./config/default.config";
	else
		config = argv[1];
	try {
		ConfigParser config_parser(config.c_str());
		config_parser.Parse();
		config_parser.PrintConf();

		webserv.SetupServer(config_parser.GetServers());
		webserv.StartServer();
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
	}
	return 0;
}
