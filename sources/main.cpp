#include <iostream>

#include "config_parser.hpp"

int main(int argc, char **argv) {
	std::string config;

	if (argc > 2)
		return 0;
	else if (argc == 1)
		config = "./config/default.config";
	else
		config = argv[1];
	try {
		ConfigParser configParser(config.c_str());
		configParser.parse();
		configParser.print_conf();
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
	}
	return 0;
}
