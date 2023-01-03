#include "server_info.hpp"
#include "config_parser.hpp"
#include "http_exception.hpp"
#include <sstream>

const std::string ServerInfo::empty_str_ = "";
const std::string ServerInfo::error_log_ = "logs/error.log";

ServerInfo::ServerInfo()
	: client_max_body_size_(1000000),
	  location_index_(-1),
	  autoindex_(false),
	  root_("./docs") {}

ServerInfo::~ServerInfo() {}

const bool &ServerInfo::GetAutoindex() const { return this->autoindex_; }

const std::string &ServerInfo::GetHost() const { return this->host_; }

const std::string &ServerInfo::GetPort() const { return this->port_; }

const std::string &ServerInfo::GetHostPort() const { return this->host_port_; }

const std::string &ServerInfo::GetRoot() {
    if (this->location_index_ == -1 || !this->locations_[location_index_].IsRoot()) {
        return this->root_;
    }
    return this->locations_[location_index_].GetRoot();
}

const std::vector<std::string> &ServerInfo::GetServerName() const {
	return this->server_name_;
}

const std::vector<std::string> &ServerInfo::GetIndex() const {
    if (location_index_ == -1 || !this->locations_[location_index_].IsIndex()) {
        return this->index_.GetIndex();
    }
    return this->locations_[location_index_].GetIndex();
}

const std::string ServerInfo::GetErrorPagePath(int status_code) {
	return this->error_pages_.GetPath(status_code);
}

const std::vector<LocationInfo> &ServerInfo::GetLocations() const {
	return this->locations_;
}

const std::string &ServerInfo::GetErrorLog() { return ServerInfo::error_log_; }

const std::vector<std::string> &ServerInfo::GetCgi() const {
    if (location_index_ == -1 || !locations_[location_index_].IsCgi()) {
        return this->cgi_;
    }
	return this->locations_[location_index_].GetCgi();
}

const std::string &ServerInfo::GetRedirect() const {
	if (location_index_ == -1) return empty_str_;
	return this->locations_[location_index_].GetRedirect();
}

int ServerInfo::GetLocationIndex() const { return this->location_index_; }

const std::string &ServerInfo::GetUploadPath() const {
	if (location_index_ == -1) return empty_str_;
	return this->locations_[location_index_].GetUploadPath();
}

// setter
void ServerInfo::SetClientMaxBodySize(int x) {
	this->client_max_body_size_ = x;
}
void ServerInfo::SetAutoindex(const bool &x) { this->autoindex_ = x; }

void ServerInfo::SetHost(const std::string &x) { this->host_ = x; }

void ServerInfo::SetPort(const std::string &x) {
	int port = atoi(x.c_str());
	if (port > PORT_MAX || port < 0) throw ConfigParser::ServerException();
	this->port_ = x;
}

void ServerInfo::SetHostPort() {
	std::string temp;
	temp = this->GetHost();
	temp += ":";
	temp += this->GetPort();
	this->host_port_ = temp;
}

void ServerInfo::SetRoot(const std::string &x) { 
	if (*(x.end()-1) != '/')
		this->root_ = x;
	else {
		int len = x.length();
		this->root_ = x.substr(0,len -1);
	}
}

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

void ServerInfo::SetErrorPages(std::string &x) { this->error_pages_.Append(x); }

void ServerInfo::SetLocations(const std::vector<LocationInfo> &x) {
	this->locations_ = x;
}

void ServerInfo::SetLocations(const LocationInfo &x) {
	this->locations_.push_back(x);
}

void ServerInfo::SetCgi(const std::string &x) { this->cgi_.push_back(x); }

void ServerInfo::SetCgi(const std::vector<std::string> &x) { this->cgi_ = x; }

void ServerInfo::SetLocationIndex(int x) { this->location_index_ = x; }

bool ServerInfo::IsServerName() const {
	if (this->server_name_.size() <= 0) return false;
	return true;
}

bool ServerInfo::IsIndex() const {
	if (location_index_ == -1 || !locations_[location_index_].IsIndex()) {
        return (this->index_.GetIndex().size() <= 0 ? false : true);
	}
	return this->locations_[location_index_].IsIndex();
}

bool ServerInfo::IsErrorPages() const {
	if (this->error_pages_.GetErrorPages().size() <= 0) return false;
	return true;
}

bool ServerInfo::IsRoot() const {
	if (this->root_.size() <= 0) return false;
	return true;
}

bool ServerInfo::IsCgi() const {
	if (location_index_ == -1 || !locations_[location_index_].IsCgi()) {
        return (this->cgi_.size() <= 0 ? false : true);
	}
	return this->locations_[location_index_].IsCgi();
}

bool ServerInfo::IsAutoIndex() const {
    if (location_index_ == -1 || !this->locations_[location_index_].GetAutoindex()) {
        return this->autoindex_;
    }
    return this->locations_[location_index_].GetAutoindex();
}

bool ServerInfo::IsRedirect() const {
	if (location_index_ == -1) return false;
	return (this->locations_[location_index_].IsRedirect());
}

bool ServerInfo::IsAllowedMethod(const std::string &x) const {
	if (location_index_ == -1)
			return true;
	return this->locations_[location_index_].IsAllowMethod(x);
}

bool ServerInfo::IsImplementedMethod(const std::string &x) const {
	if ((x == "GET") || (x == "POST") || (x == "DELETE"))
		return true;
	return false;
}

const std::string ServerInfo::GetPath() const {
	if (location_index_ == -1) {
		return empty_str_;
	}
	return this->locations_[location_index_].GetPathNoSlash();
}

std::vector<std::string> ServerInfo::GetAllowedMethod() const {
	std::vector<std::string> temp;
	if (location_index_ == -1) return temp;
	return this->locations_[location_index_].GetAllowMethods();
}

size_t ServerInfo::GetClientMaxBodySize() const {
	int temp;
	if (location_index_ == -1)
		temp = client_max_body_size_;
	else
		temp = this->locations_[location_index_].GetClientMaxBodySize();
	return temp;
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
	ss << index_;
	ss << '\n';
	ss << C_NOFAINT << "=  error_pages : " << C_FAINT << '\n';
	ss << error_pages_;
	ss << C_NOFAINT << "=  Iscgi   : " << C_FAINT << IsCgi() << '\n';
	ss << C_NOFAINT << "=  IsIndex : " << C_FAINT << IsIndex() << '\n';
	ss << C_NOFAINT << "=  IsServerIndex : " << C_FAINT << IsIndex() << '\n';
	ss << C_NOFAINT << "=  IsErrorPages  : " << C_FAINT << IsErrorPages()
	   << '\n';
	ss << C_NOFAINT << "=  IsRoot        : " << C_FAINT << IsRoot() << '\n';

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
