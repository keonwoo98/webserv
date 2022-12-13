#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <fstream>
#include <vector>

#include "server.hpp"

#define PORT_MAX 65535

class ConfigParser {
   private:
	std::string config_;
	std::vector<Server> server_;

   public:
	ConfigParser(const char *file);
	~ConfigParser();
	void parse();
	Server parse_server(size_t &i);
	Location parse_location(size_t &i);
	void set_server(Server &server, std::string key, std::string value);
	void set_location(Location &location, std::string key, std::string value);
	std::vector<std::string> split(std::string input, char delimiter);
	void print_conf(void);

	class FstreamException : public std::exception {
	   public:
		const char *what() const throw() {
			return "Config Parse Error: Failed to open config file.";
		}
	};

	class BracketException : public std::exception {
	   public:
		const char *what() const throw() {
			return "Config Parse Error: Unclosed brackets detected.";
		}
	};

	class NoContentException : public std::exception {
	   public:
		const char *what() const throw() {
			return "Config Parse Error: No content in the config file.";
		}
	};

	class ServerException : public std::exception {
	   public:
		const char *what() const throw() {
			return "Config Parse Error: Invalid server block.";
		}
	};

	class LocationException : public std::exception {
	   public:
		const char *what() const throw() {
			return "Config Parse Error: Invalid location block.";
		}
	};
};

#endif
