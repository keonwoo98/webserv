#include <iostream>
#include <sstream>

#include "config_parser.hpp"
#include "character_color.hpp"

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

void ConfigParser::Parse(servers_type &server_blocks) {
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

Server ConfigParser::ParseServer(size_t &i) {
	std::string key;
	std::string value;
	Server server;
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

Location ConfigParser::ParseLocation(size_t &i) {
	std::string key;
	std::string value;
	Location location;

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

void ConfigParser::SetServer(Server &server, std::string key,
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
		std::vector<std::string> temp = Split(value, ' ');
		for (size_t i = 0; i != temp.size(); i++)
			server.SetIndex(temp[i]);
	} else if (key == "allow_methods") {
		std::vector<std::string> temp = Split(value, ' ');
		for (size_t i = 0; i < temp.size(); i++) {
			if (temp[i] != "GET" && temp[i] != "POST" && temp[i] != "DELETE")
				server.SetAllowMethods("INVALID");
			else
				server.SetAllowMethods(temp[i]);
		}
	} else if (key == "error_page") {
		std::vector<std::string> temp = Split(value, ' ');
		std::vector<int> pages; // 얘 어디서 쓰이지?
		std::string path = temp[temp.size() - 1];
		for (size_t i = 0; i < temp.size() - 1; i++) {
			server.SetErrorPages(std::pair<int, std::string>(atoi(temp[i].c_str()), path));
		}
	}
}

void ConfigParser::SetLocation(Location &location, std::string key,
								std::string value) {
	if (key == "root") {
		location.SetRoot(value);
	} else if (key == "index") {
		std::vector<std::string> temp = Split(value, ' ');
		for (size_t i = 0; i != temp.size(); i++)
			location.SetIndex(temp[i]);
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

std::vector<std::string> ConfigParser::Split(std::string input,
											 char delimiter) {
	std::vector<std::string> str;
	std::stringstream ss(input);
	std::string temp;

	while (std::getline(ss, temp, delimiter)) str.push_back(temp);

	return str;
}

void ConfigParser::PrintConf(servers_type &server_blocks) {
	std::cout << C_BOLD << C_ITALIC << C_GREEN;
	std::cout << "   [Server Configuration Info]   " << C_NOBOLD << C_NOITALIC << std::endl;
	for (size_t i = 0; i < server_blocks.size(); i++) {
		std::cout << C_NOFAINT << "=========server " << i + 1 << "=========" << std::endl;
		std::cout << C_NOFAINT << "=  servers_name : " << C_FAINT;
		for ( size_t j = 0; j < server_blocks[i].GetServerName().size(); j++)
			std::cout << server_blocks[i].GetServerName()[j] << ' ';
		std::cout << std::endl;
		std::cout << C_NOFAINT << "=  host : " << C_FAINT << server_blocks[i].GetHost() << std::endl;
		std::cout << C_NOFAINT << "=  port : " << C_FAINT << server_blocks[i].GetPort() << std::endl;
		std::cout << C_NOFAINT << "=  root : " << C_FAINT << server_blocks[i].GetRoot() << std::endl;
		std::cout << C_NOFAINT << "=  autoindex : " << C_FAINT << server_blocks[i].GetAutoindex() << std::endl;
		std::cout << C_NOFAINT << "=  client_max_body_size : " << C_FAINT 
				  << server_blocks[i].GetClientMaxBodySize() << std::endl;
		std::cout << C_NOFAINT << "=  index : " << C_FAINT;
		for (size_t j = 0; j < server_blocks[i].GetIndex().size(); j++)
			std::cout << server_blocks[i].GetIndex()[j] << ' ';
		std::cout << std::endl;

		std::cout << C_NOFAINT << "=  allow_methods : " << C_FAINT;
		for (size_t j = 0; j < server_blocks[i].GetAllowMethods().size(); j++)
			std::cout << server_blocks[i].GetAllowMethods()[j] << ' ';
		std::cout << std::endl;
		std::cout << C_NOFAINT << "=  error_pages : " << C_FAINT << std::endl;
		for (std::map<int, std::string>::const_iterator it =
				 server_blocks[i].GetErrorPages().begin();
			 it != server_blocks[i].GetErrorPages().end(); it++) {
			std::cout << "      " << it->first << ' ' << it->second << std::endl;
		}
		std::cout << C_NOFAINT << "=  IsServerIndex : " << C_FAINT << server_blocks[i].IsIndex(); 
		std::cout << std::endl;
		std::cout << C_NOFAINT << "=  IsErrorPages : " << C_FAINT << server_blocks[i].IsErrorPages();
		std::cout << std::endl;

		for (size_t j = 0; j < server_blocks[i].GetLocations().size(); j++) {
			std::cout << C_NOFAINT << "=  location " << j + 1 << C_FAINT << std::endl;
			std::cout << "      path : " << server_blocks[i].GetLocations()[j].GetPath() << std::endl;
			std::cout << "      root : " << server_blocks[i].GetLocations()[j].GetRoot() << std::endl;
			std::cout << "      client_max_body_size : "
					  << server_blocks[i].GetLocations()[j].GetClientMaxBodySize() << std::endl;
			std::cout << "      index : ";
			for (size_t k = 0; k < server_blocks[i].GetLocations()[j].GetIndex().size(); k++)
				std::cout << server_blocks[i].GetLocations()[j].GetIndex()[k] << ' ';
			std::cout << std::endl;
			std::cout << "      allow_methods : ";
			for (size_t k = 0; k < server_blocks[i].GetLocations()[j].GetAllowMethods().size();
				 k++)
				std::cout << server_blocks[i].GetLocations()[j].GetAllowMethods()[k] << ' ';
			std::cout << std::endl;
			std::cout << "      cgi : ";
			for (size_t k = 0; k < server_blocks[i].GetLocations()[j].GetCgi().size(); k++)
				std::cout << server_blocks[i].GetLocations()[j].GetCgi()[k] << ' ';
			std::cout << std::endl;
			std::cout << "      Iscgi : " << server_blocks[i].GetLocations()[j].IsCgi();
			std::cout << std::endl;
			std::cout << "      IsIndex : " << server_blocks[i].GetLocations()[j].IsIndex();
			std::cout << std::endl;
		}
		std::cout << C_NOFAINT << "==========================" << std::endl;
	}
	std::cout << C_RESET << std::endl;
}
