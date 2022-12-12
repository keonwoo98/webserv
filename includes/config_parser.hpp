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
	void Parse();
	Server ParseServer(size_t *i);
	Location ParseLocation(size_t *i);
	void SetServer(Server *server, std::string key, std::string value);
	void SetLocation(Location *location, std::string key, std::string value);
	std::vector<std::string> Split(std::string input, char delimiter);
	void PrintConf(void);

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
