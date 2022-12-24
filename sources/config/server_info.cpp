#include "server_info.hpp"
#include "character_color.hpp"

#include <sstream>

ServerInfo::ServerInfo() : client_max_body_size_(1000000), autoindex_(false) {}

ServerInfo::~ServerInfo() {}

const int &ServerInfo::GetClientMaxBodySize() const {
	return this->client_max_body_size_;
}
const bool &ServerInfo::GetAutoindex() const { return this->autoindex_; }
const std::string &ServerInfo::GetHost() const { return this->host_; }
const std::string &ServerInfo::GetPort() const { return this->port_; }
const std::string &ServerInfo::GetHostPort() const { return this->host_port_; }
const std::string &ServerInfo::GetRoot() const { return this->root_; }
const std::vector<std::string> &ServerInfo::GetServerName() const {
	return this->server_name_;
}
const std::vector<std::string> &ServerInfo::GetIndex() const {
	return this->index_.GetIndex();
}
const std::map<int, std::string> &ServerInfo::GetErrorPages() const {
	return this->error_pages_.GetErrorPages();
}
const std::vector<LocationInfo> &ServerInfo::GetLocations() const {
	return this->locations_;
}

// setter
void ServerInfo::SetClientMaxBodySize(const int &x) {
	this->client_max_body_size_ = x;
}
void ServerInfo::SetAutoindex(const bool &x) { this->autoindex_ = x; }
void ServerInfo::SetHost(const std::string &x) { this->host_ = x; }
void ServerInfo::SetPort(const std::string &x) { this->port_ = x; }
void ServerInfo::SetHostPort() {
	std::string temp;
	temp = this->GetHost();
	temp += ":";
	temp += this->GetPort();
	this->host_port_ = temp;
}
void ServerInfo::SetRoot(const std::string &x) { this->root_ = x; }
void ServerInfo::SetServerName(const std::vector<std::string> &x) {
	this->server_name_ = x;
}
void ServerInfo::SetServerName(const std::string &x) {
	this->server_name_.push_back(x);
}
void ServerInfo::SetIndex(std::string &x) {
	Index index(x);
	this->index_ = index;
}
void ServerInfo::SetErrorPages(std::string &x) {
	this->error_pages_.Append(x);
}
void ServerInfo::SetLocations(const std::vector<LocationInfo> &x) {
	this->locations_ = x;
}
void ServerInfo::SetLocations(const LocationInfo &x) {
	this->locations_.push_back(x);
}
bool ServerInfo::IsServerName() const {
	if (this->server_name_.size() <= 0) return false;
	return true;
}
bool ServerInfo::IsIndex() const {
	if (this->index_.GetIndex().size() <= 0) return false;
	return true;
}

bool ServerInfo::IsErrorPages() const {
	if (this->error_pages_.GetErrorPages().size() <= 0) return false;
	return true;
}
bool ServerInfo::IsRoot() const {
	if (this->root_.size() <= 0) return false;
	return true;
}

std::string ServerInfo::ToString() const {
	std::stringstream ss;

	ss << C_NOFAINT << "servers_name : " << C_FAINT;
	for (size_t i = 0; i < server_name_.size(); i++)
		ss << server_name_[i] << ' ';
	ss << '\n';
	ss << C_NOFAINT << "=  host : " << C_FAINT << host_ << '\n';
	ss << C_NOFAINT << "=  port : " << C_FAINT << port_ << '\n';
	ss << C_NOFAINT << "=  root : " << C_FAINT << root_ << '\n';
	ss << C_NOFAINT << "=  autoindex : " << C_FAINT << autoindex_ << '\n';
	ss << C_NOFAINT << "=  client_max_body_size : " << C_FAINT
	   << client_max_body_size_ << '\n';
	ss << C_NOFAINT << "=  index : " << C_FAINT;
	for (size_t i = 0; i < index_.GetIndex().size(); i++)
		ss << index_.GetIndex()[i] << ' ';
	ss << '\n';
	ss << C_NOFAINT << "=  error_pages : " << C_FAINT << '\n';
	for (std::map<int, std::string>::const_iterator it =
			 error_pages_.GetErrorPages().begin();
		 it != error_pages_.GetErrorPages().end(); it++) {
		ss << "\t" << it->first << ' ' << it->second << '\n';
	}
	ss << C_NOFAINT << "=  IsServerIndex : " << C_FAINT << IsIndex() << '\n';
	ss << C_NOFAINT << "=  IsErrorPages  : " << C_FAINT << IsErrorPages() << '\n';
	ss << C_NOFAINT << "=  IsRoot        : " << C_FAINT << IsRoot()
	   << '\n';

	for (size_t i = 0; i < locations_.size(); i++) {
		ss << locations_[i];
		if (i + 1 != locations_.size()) {
			ss << '\n';
		}
	}
	return ss.str();
}

std::ostream &operator<<(std::ostream &out, const ServerInfo &server) {
	out << server.ToString();
	return out;
}
