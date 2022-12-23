#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <fstream>
#include <vector>

#include "server_info.hpp"

class ConfigParser {
   public:
	static std::string white_spaces;
	typedef std::vector<ServerInfo> servers_type;
	typedef std::map<std::string, std::vector<ServerInfo> > use_type;
	explicit ConfigParser(const char *file);
	~ConfigParser();

	void SetServer(ServerInfo &server, std::string key, std::string value);
	void SetLocation(LocationInfo &location, std::string key, std::string value);

	void Parse(servers_type &server_blocks);
	void PrintConf(servers_type &server_blocks);

	void ParseUse(use_type &use_map,std::vector<ServerInfo> &server_blocks);
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

   private:
	std::string config_;

	ServerInfo ParseServer(size_t &i);
	LocationInfo ParseLocation(size_t &i);
};

#endif
