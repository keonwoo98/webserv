#include <iostream>
#include <sstream>

#include "config_parser.hpp"
#include "character_color.hpp"
#include "config_utils.hpp"

std::string ConfigParser::white_spaces = " \r\n\t\v\f";

std::string &ltrim(std::string &str) {
	str.erase(0, str.find_first_not_of(ConfigParser::white_spaces));
	return str;
}

std::string &rtrim(std::string &str) {
	str.erase(str.find_last_not_of(ConfigParser::white_spaces) + 1);
	return str;
}

std::string &trim(std::string &str) {
	return ltrim(rtrim(str));
}

ConfigParser::ConfigParser(const char *file) {
	std::ifstream in(file);
	std::string line;

	if (!in) {
		throw FstreamException();
	}
	int bracket = 0;
	config_.clear();
	while (std::getline(in, line)) {
		trim(line);
		if (line[0] == '#' || line.length() <= 0) continue;
		config_.append(line + '\n');
		for (size_t i = 0; i < line.length(); i++) {
			if (line[i] == '{')
				bracket++;
			else if (line[i] == '}')
				bracket--;
		}
		char eol = line[line.length() - 1];
		if (eol != '{' && eol != '}' && eol != ';') throw BracketException();
	}
	in.close();
	if (bracket) throw BracketException();
}

ConfigParser::~ConfigParser() {}

void ConfigParser::Parse(server_infos_type &server_blocks) {
	size_t pre = 0;
	size_t cur = 0;
	while (cur != std::string::npos) {
		pre = config_.find_first_not_of(white_spaces, cur);
		cur = config_.find_first_of(" \t\n\v\f\r{", pre);
		std::string key = config_.substr(pre, cur - pre);
		if (key != "server") {
			throw ServerException();
		}
		server_blocks.push_back(ParseServer(cur));
	}
}

ServerInfo ConfigParser::ParseServer(size_t &i) {
	std::string key;
	std::string value;
	ServerInfo server;
	size_t pre = config_.find_first_not_of(white_spaces, i);
	if (pre == std::string::npos || config_[pre] != '{') {
		throw ServerException();
	}
	pre++;
	size_t cur = config_.find_first_not_of(white_spaces, pre);
	while (cur != std::string::npos) {
		if ((pre = config_.find_first_not_of(white_spaces, cur)) ==
			std::string::npos) {
			throw ServerException();
		}
		if ((cur = config_.find_first_of(white_spaces, pre)) ==
			std::string::npos) {
			throw ServerException();
		}
		key = config_.substr(pre, cur - pre);
		if (key == "}") {
			i = config_.find_first_not_of(white_spaces, cur + 1);
			break;
		}
		if (key == "location") {
			server.SetLocations((ParseLocation(cur)));
		} else {
			if ((pre = config_.find_first_not_of(white_spaces, cur)) ==
				std::string::npos) {
				throw ServerException();
			}
			if ((cur = config_.find_first_of("\n", pre)) == std::string::npos) {
				throw ServerException();
			}
			value = config_.substr(pre, cur - pre - 1);
			SetServer(server, key, value);
		}
	}
	return server;
}

LocationInfo ConfigParser::ParseLocation(size_t &i) {
	std::string key;
	std::string value;
	LocationInfo location;

	size_t pre = config_.find_first_not_of(white_spaces, i);
	size_t cur = config_.find_first_of(white_spaces, pre);
	location.SetPath(config_.substr(pre, cur - pre));

	pre = config_.find_first_not_of(white_spaces, cur);
	if (pre == std::string::npos || config_[pre] != '{') {
		throw LocationException();
	}
	pre++;
	cur = config_.find_first_of(white_spaces, pre);
	while (cur != std::string::npos) {
		if ((pre = config_.find_first_not_of(white_spaces, cur)) ==
			std::string::npos) {
			throw LocationException();
		}
		if ((cur = config_.find_first_of(white_spaces, pre)) ==
			std::string::npos) {
			throw LocationException();
		}
		key = config_.substr(pre, cur - pre);
		if (key == "}") {
			i = config_.find_first_not_of(white_spaces, cur + 1);
			break;
		} else {
			if ((pre = config_.find_first_not_of(white_spaces, cur)) ==
				std::string::npos) {
				throw LocationException();
			}
			if ((cur = config_.find_first_of("\n", pre)) == std::string::npos) {
				throw LocationException();
			}
			value = config_.substr(pre, cur - pre - 1);
			SetLocation(location, key, value);
		}
	}
	return location;
}

void ConfigParser::SetServer(ServerInfo &server, std::string key,
							 std::string value) {
	if (key == "server_name") {
		std::vector<std::string> temp = Split(value, ' ');
		for (size_t i = 0; i != temp.size(); i++)
			server.SetServerName(temp[i]);
	} else if (key == "listen") {
		if (value.find_first_of(":") == std::string::npos) {
			if (value.find_first_of(".") == std::string::npos) {
				server.SetHost("0.0.0.0");
				server.SetPort(value);
			} else {
				server.SetPort("8000");
				server.SetHost(value);
			}
		} else {
			std::vector<std::string> temp = Split(value, ':');
			if (temp[0] == "*")
				temp[0] = "0.0.0.0";
			if (temp[1] == "*")
				temp[1] = "0.0.0.0";
			server.SetHost(temp[0]);
			server.SetPort(temp[1]);
		}
		server.SetHostPort();
	} else if (key == "client_max_body_size") {
		server.SetClientMaxBodySize(atoi(value.c_str()));
	} else if (key == "root") {
		server.SetRoot(value);
	} else if (key == "autoindex") {
		if (value == "on")
			server.SetAutoindex(true);
		else
			server.SetAutoindex(false);
	} else if (key == "index") {
		server.SetIndex(value);
	} else if (key == "error_page") {
		server.SetErrorPages(value);
	}
}

void ConfigParser::SetLocation(LocationInfo &location, std::string key,
							   std::string value) {
	if (key == "root") {
		location.SetRoot(value);
	} else if (key == "index") {
		location.SetIndex(value);
	} else if (key == "error_page") {
		location.SetErrorPages(value);
	} else if (key == "allow_methods") {
		std::vector<std::string> temp = Split(value, ' ');
		for (size_t i = 0; i < temp.size(); i++) {
			if (temp[i] != "GET" && temp[i] != "POST" && temp[i] != "DELETE")
				location.SetAllowMethods("INVAILD");
			else
				location.SetAllowMethods(temp[i]);
		}
	} else if (key == "cgi") {
		std::vector<std::string> temp = Split(value, ' ');
		for (size_t i = 0; i != temp.size(); i++)
			location.SetCgi(temp[i]);
	} else if (key == "client_max_body_size") {
		location.SetClientMaxBodySize(atoi(value.c_str()));
	} else {
		throw LocationException();
	}
}

void ConfigParser::PrintConf(server_infos_type &server_blocks) {
	std::cout << C_BOLD << C_GREEN << "[Server Configuration Info]" << C_NOBOLD << C_NOITALIC
			  << std::endl;
	for (size_t i = 0; i < server_blocks.size(); i++) {
		std::cout << C_NOFAINT << "=========server " << i + 1 << "=========" << std::endl;
		std::cout << server_blocks[i] << std::endl;
	}
	std::cout << C_RESET;
}

void ConfigParser::ParseConfigs(server_configs_type &server_configs, std::vector<ServerInfo> &server_blocks) {
	std::vector<ServerInfo> server_temp;
	server_configs_type::iterator it;
	for (size_t i = 0; i < server_blocks.size(); i++) {
		it = server_configs.find(server_blocks[i].GetHostPort());
		if (it == server_configs.end()) {
			server_temp.clear();
			server_temp.push_back(server_blocks[i]);
			server_configs.insert(make_pair(server_blocks[i].GetHostPort(), server_temp));
		} else {
			server_temp.push_back(server_blocks[i]);
			server_configs.erase(it);
			server_configs.insert(make_pair(server_blocks[i].GetHostPort(), server_temp));
		}
	}
	for (server_configs_type::iterator se = server_configs.begin(); se != server_configs.end(); se++) {
		for (size_t i = 0; i < se->second.size(); i++)
			std::cout << i << " " << se->first << "|" << se->second[i].GetClientMaxBodySize() << std::endl;
	}
}
