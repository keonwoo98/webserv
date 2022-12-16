#include <iostream>

#include "config_parser.hpp"

// int main(int argc, char **argv) {
// 	std::string config;

// 	if (argc > 2)
// 		return 0;
// 	else if (argc == 1)
// 		config = "./config/default.config";
// 	else
// 		config = argv[1];
// 	try {
// 		ConfigParser configParser(config.c_str());
// 		configParser.Parse();
// 		configParser.PrintConf();
// 	} catch (const std::exception &e) {
// 		std::cerr << e.what() << '\n';
// 	}
// 	return 0;
// }

#include "webserv.hpp"

int main(int argc, char **argv) {
	std::string config;
	// conf 파일도 confstart() 해서 시작해서 하는걸로 하자.
	if (argc > 2)
		return 0;
	else if (argc == 1)
		config = "./config/default.config";
	else
		config = argv[1];
	try {
		ConfigParser configParser(config.c_str());
		configParser.Parse();
		configParser.PrintConf();
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
	}
	Webserv webserv;

	webserv.SetupServer();
	webserv.StartServer();
}
