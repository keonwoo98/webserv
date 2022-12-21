#include "server.hpp"
#include "character_color.hpp"
#include <iostream>

Server::Server(): client_max_body_size_(1000000), autoindex_(false) {}

Server::~Server() {}

const int &Server::GetClientMaxBodySize() const {
    return this->client_max_body_size_;
}
const bool &Server::GetAutoindex() const{
    return this->autoindex_;
}
const std::string &Server::GetHost() const{
    return this->host_;
}
const std::string &Server::GetPort() const{
    return this->port_;
}
const std::string &Server::GetRoot() const{
    return this->root_;
}
const std::vector<std::string> &Server::GetServerName() const{
    return this->server_name_;
}
const std::vector<std::string> &Server::GetIndex() const{
    return this->index_;
}
const std::vector<std::string> &Server::GetAllowMethods() const{
    return this->allow_methods_;
}
const std::map<int, std::string> &Server::GetErrorPages() const{
    return this->error_pages_;
}
const std::vector<Location> &Server::GetLocations() const{
    return this->locations_;
}

// setter
void Server::SetClientMaxBodySize(const int &x){
    this->client_max_body_size_ = x;
}
void Server::SetAutoindex(const bool &x){
    this->autoindex_ = x;
}
void Server::SetHost(const std::string &x){
    this->host_ = x;
}
void Server::SetPort(const std::string &x){
    this->port_ = x;
}
void Server::SetRoot(const std::string &x){
    this->root_ = x;
}
void Server::SetServerName(const std::vector<std::string> &x){
    this->server_name_ = x;
}
void Server::SetServerName(const std::string &x){
    this->server_name_.push_back(x);
}
void Server::SetIndex(const std::vector<std::string> &x){
    this->index_ = x;
}
void Server::SetIndex(const std::string &x){
    this->index_.push_back(x);
}
void Server::SetAllowMethods(const std::vector<std::string> &x){
    this->allow_methods_ = x;
}
void Server::SetAllowMethods(const std::string &x){
    this->allow_methods_.push_back(x);
}
void Server::SetErrorPages(const std::map<int, std::string> &x){
    this->error_pages_ = x;
}

void Server::SetErrorPages(const std::pair<int, std::string> &x){
    this->error_pages_.insert(x);
}
void Server::SetLocations(const std::vector<Location> &x){
    this->locations_ = x;
}
void Server::SetLocations(const Location &x){
    this->locations_.push_back(x);
}
bool Server::IsServerName() const{
    if(this->server_name_.size() <= 0)
        return false;
    return true;
}
bool Server::IsIndex() const{
    if (this->index_.size() <= 0)
        return false;
    return true;
}

bool Server::IsErrorPages() const{
    if (this->error_pages_.size() <= 0)
        return false;
    return true;
}

void Server::print() const {
	std::cout << C_BOLD << C_ITALIC << C_GREEN;
	std::cout << "   [Server Configuration Info]   " << C_NOBOLD << C_NOITALIC
			  << std::endl;
	std::cout << C_NOFAINT << "=  servers_name : " << C_FAINT;
	for (size_t i = 0; i < GetServerName().size(); i++)
		std::cout << GetServerName()[i] << ' ';
	std::cout << std::endl;
	std::cout << C_NOFAINT << "=  host : " << C_FAINT << GetHost() << std::endl;
	std::cout << C_NOFAINT << "=  port : " << C_FAINT << GetPort() << std::endl;
	std::cout << C_NOFAINT << "=  root : " << C_FAINT << GetRoot() << std::endl;
	std::cout << C_NOFAINT << "=  autoindex : " << C_FAINT << GetAutoindex()
			  << std::endl;
	std::cout << C_NOFAINT << "=  client_max_body_size : " << C_FAINT
			  << GetClientMaxBodySize() << std::endl;
	std::cout << C_NOFAINT << "=  index : " << C_FAINT;
	for (size_t i = 0; i < GetIndex().size(); i++)
		std::cout << GetIndex()[i] << ' ';
	std::cout << std::endl;

	std::cout << C_NOFAINT << "=  allow_methods : " << C_FAINT;
	for (size_t i = 0; i < GetAllowMethods().size(); i++)
		std::cout << GetAllowMethods()[i] << ' ';
	std::cout << std::endl;
	std::cout << C_NOFAINT << "=  error_pages : " << C_FAINT << std::endl;
	for (std::map<int, std::string>::const_iterator it = GetErrorPages().begin();
		 it != GetErrorPages().end(); it++) {
		std::cout << "      " << it->first << ' ' << it->second << std::endl;
	}
	std::cout << C_NOFAINT << "=  IsServerIndex : " << C_FAINT << IsIndex();
	std::cout << std::endl;
	std::cout << C_NOFAINT << "=  IsErrorPages : " << C_FAINT << IsErrorPages();
	std::cout << std::endl << std::endl;

	for (size_t i = 0; i < GetLocations().size(); i++) {
		std::cout << GetLocations()[i] << std::endl;
	}
    std::cout << C_NOFAINT << "======================================" << C_RESET << std::endl;
}
