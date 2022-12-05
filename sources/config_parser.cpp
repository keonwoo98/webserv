#include "config_parser.hpp"
#include <iostream>
#include <sstream>

ConfigParser::ConfigParser(const char *file) {
	std::ifstream in(file);
	std::string line;

	if (!in) {
		throw FstreamException();
	}
	int bracket = 0;
	config_.clear();
	while (std::getline(in, line)) {
		if (line.find("#") == std::string::npos &&
			line.find_first_not_of(" \t\n\v\f\r") != std::string::npos)
			config_.append(line + '\n');
		for (int i = 0; i < line.length(); i++)
		{
			if (line[i] == '{')
				bracket++;
			else if (line[i] == '}')
				bracket--;
		}
		char eol = line[line.length() - 1];
		if (eol != '{' && eol != '}' && eol != ';' && eol != '\0' && eol != '\t')
			throw BracketException();
	}
	in.close();
	if (bracket)
		throw BracketException();
}

ConfigParser::~ConfigParser() {}

void ConfigParser::parse() {
	size_t pre = 0;
	size_t cur = config_.find_first_not_of(" \t\n\v\f\r", pre);
	if (cur == std::string::npos) {
		throw NoContentException();
	}
	while (cur != std::string::npos) {
		pre = config_.find_first_not_of(" \t\n\v\f\r", cur);
		cur = config_.find_first_of(" \t\n\v\f\r{", pre);
		std::string key = config_.substr(pre, cur - pre);
		if (key != "server") {
			throw ServerException();
		}
		server_.push_back(parse_server(&cur));
	}
}

Server ConfigParser::parse_server(size_t *i) {
	std::string key;
	std::string value;
	Server server;

	size_t pre = config_.find_first_not_of(" \t\n\v\f\r", *i);
	if (pre == std::string::npos || config_[pre] != '{') {
		throw ServerException();
	}
	pre++;
	size_t cur = config_.find_first_not_of(" \t\n\v\f\r", pre);
	while (cur != std::string::npos) {
		if ((pre = config_.find_first_not_of(" \t\n\v\f\r", cur)) ==
			std::string::npos) {
			throw ServerException();
		}
		if ((cur = config_.find_first_of(" \t\n\v\f\r", pre)) == std::string::npos) {
			throw ServerException();
		}
		key = config_.substr(pre, cur - pre);
		if (key == "}") {
			*i = config_.find_first_not_of(" \t\n\v\f\r", cur + 1);
			break;
		}
		if (key == "location") {
			server.locations_.push_back(parse_location(&cur));
		} else {
			if ((pre = config_.find_first_not_of(" \t\n\v\f\r", cur)) ==
				std::string::npos) {
				throw ServerException();
			}
			if ((cur = config_.find_first_of("\n", pre)) == std::string::npos) {
				throw ServerException();
			}
			value = config_.substr(pre, cur - pre - 1);
			set_server(&server, key, value);
		}
	}
 	return server;
}

Location ConfigParser::parse_location(size_t *i) {
	std::string key;
	std::string value;
	Location location;

	size_t pre = config_.find_first_not_of(" \t\n\v\f\r", *i);
	size_t cur = config_.find_first_of(" \t\n\v\f\r", pre);
	location.path_ = config_.substr(pre, cur - pre);

	pre = config_.find_first_not_of(" \t\n\v\f\r", cur);
	if (pre == std::string::npos || config_[pre] != '{') {
		throw LocationException();
	}
	pre++;
	cur = config_.find_first_of(" \t\n\v\f\r", pre);
	while (cur != std::string::npos) {
		if ((pre = config_.find_first_not_of(" \t\n\v\f\r", cur)) ==
			std::string::npos) {
			throw LocationException();
		}
		if ((cur = config_.find_first_of(" \t\n\v\f\r", pre)) == std::string::npos) {
			throw LocationException();
		}
		key = config_.substr(pre, cur - pre);
		if (key == "}") {
			*i = config_.find_first_not_of(" \t\n\v\f\r", cur + 1);
			break;
		} else {
			if ((pre = config_.find_first_not_of(" \t\n\v\f\r", cur)) ==
				std::string::npos) {
				throw LocationException();
			}
			if ((cur = config_.find_first_of("\n", pre)) == std::string::npos) {
				throw LocationException();
			}
			value = config_.substr(pre, cur - pre - 1);
			set_location(&location, key, value);
		}
	}
	return location;
}

void ConfigParser::set_server(Server *server, std::string key,
							  std::string value) {
	if (key == "server_name") {
		server->server_name_ = value;
	} else if (key == "listen") {
		if (value.find_first_of(":") == std::string::npos) {
			server->host_ = "0.0.0.0";
			server->port_ = value;
		} else {
			std::vector<std::string> temp = split(value, ':');
			server->host_ = temp[0];
			server->port_ = temp[1];
		}
	} else if (key == "client_max_body_size") {
		server->client_max_body_size_ = atoi(value.c_str());
	} else if (key == "root") {
		server->root_ = value;
	} else if (key == "autoindex") {
		if (value == "on")
			server->autoindex_ = true;
		else
			server->autoindex_ = false;
	} else if (key == "index") {
		std::vector<std::string> temp = split(value, ' ');
		for (int i = 0; i != temp.size(); i++)
			server->index_.push_back(temp[i]);
	} else if (key == "allow_methods") {
		std::vector<std::string> temp = split(value, ' ');
		for (int i = 0; i < temp.size(); i++) {
			if (temp[i] != "GET" && temp[i] != "POST" && temp[i] != "DELETE")
				server->allow_methods_.push_back("INVALID");
			else
				server->allow_methods_.push_back(temp[i]);
		}
	} else if (key == "error_page") {
		std::vector<std::string> temp = split(value, ' ');
		std::vector<int> pages;
		std::string path = temp[temp.size() - 1];
		for (int i = 0; i < temp.size() - 1; i++) {
			server->error_pages_.insert(std::pair<int, std::string>(atoi(temp[i].c_str()), path));
		}
	}
}

void ConfigParser::set_location(Location *location, std::string key,
								std::string value) {
	if (key == "root") {
		location->root_ = value;
	} else if (key == "index") {
		std::vector<std::string> temp = split(value, ' ');
		for (int i = 0; i != temp.size(); i++)
			location->index_.push_back(temp[i]);
	} else if (key == "allow_methods") {
		std::vector<std::string> temp = split(value, ' ');
		for (int i = 0; i < temp.size(); i++) {
			if (temp[i] != "GET" && temp[i] != "POST" && temp[i] != "DELETE")
				location->allow_methods_.push_back("INVALID");
			else
				location->allow_methods_.push_back(temp[i]);
		}
	} else if (key == "cgi") {
		std::vector<std::string> temp = split(value, ' ');
		for (int i = 0; i != temp.size(); i++)
			location->cgi_.push_back(temp[i]);
	} else if (key == "client_body_limit") {
		location->client_max_body_size_ = atoi(value.c_str());
	} else {
		throw LocationException();
	}
}

std::vector<std::string> ConfigParser::split(std::string input,
											 char delimiter) {
	std::vector<std::string> str;
	std::stringstream ss(input);
	std::string temp;

	while (std::getline(ss, temp, delimiter)) str.push_back(temp);

	return str;
}

void ConfigParser::print_conf() {
	for (int i = 0; i < server_.size(); i++) {
		std::cout << "server " << i + 1 <<'\n';
		std::cout << "server_name : " << server_[i].server_name_ << '\n';
		std::cout << "host : " << server_[i].host_ << '\n';
		std::cout << "port : " << server_[i].port_ << '\n';
		std::cout << "root : " << server_[i].root_ << '\n';
		std::cout << "autoindex : " << server_[i].autoindex_ << '\n';
		std::cout << "client_max_body_size : " << server_[i].client_max_body_size_<< '\n';
		std::cout << "index : ";
		for (int j = 0; j < server_[i].index_.size(); j++)
			std::cout << server_[i].index_[j] << ' ';
		std::cout << '\n';
		std::cout << "allow_methods : ";
		for (int j = 0; j < server_[i].allow_methods_.size(); j++)
			std::cout << server_[i].allow_methods_[j] << ' ';
		std::cout << '\n';
		std::cout << "error_pages : " << '\n';
		for (std::map<int, std::string>::iterator it = server_[i].error_pages_.begin(); it != server_[i].error_pages_.end(); it++) {
			std::cout << it->first << ' ' << it->second << '\n';
		}
		std::cout << "\n\n";
		for (int j = 0; j < server_[i].locations_.size(); j++) {
			std::cout << "location " << j + 1 << '\n';
			std::cout << "path : " << server_[i].locations_[j].path_ << '\n';
			std::cout << "root : " << server_[i].locations_[j].root_ << '\n';
			std::cout << "client_max_body_size : " << server_[i].locations_[j].client_max_body_size_ << '\n';
			std::cout << "index : ";
			for (int k = 0; k < server_[i].locations_[j].index_.size(); k++)
				std::cout << server_[i].locations_[j].index_[k] << ' ';
			std::cout << '\n';
			std::cout << "allow_methods : ";
			for (int k = 0; k < server_[i].locations_[j].allow_methods_.size(); k++)
				std::cout << server_[i].locations_[j].allow_methods_[k] << ' ';
			std::cout << '\n';
			std::cout << "cgi : ";
			for (int k = 0; k < server_[i].locations_[j].cgi_.size(); k++)
				std::cout << server_[i].locations_[j].cgi_[k] << ' ';
			std::cout << "\n\n";
		}
		std::cout << "\n\n";
	}
}
