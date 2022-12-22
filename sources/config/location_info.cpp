#include "location_info.hpp"
#include "character_color.hpp"
#include <iostream>

LocationInfo::LocationInfo() : client_max_body_size_(1000000), path_(""), root_("./docs/index.html") {}

LocationInfo::~LocationInfo() {}
// getter 
const int &LocationInfo::GetClientMaxBodySize() const {
    return this->client_max_body_size_;
}

const std::string &LocationInfo::GetPath() const {
    return this->path_;
}

const std::string &LocationInfo::GetRoot() const {
    return this->root_;
}

const std::vector<std::string> &LocationInfo::GetIndex() const {
    return this->index_;
}

const std::vector<std::string> &LocationInfo::GetAllowMethods() const {
    return this->allow_methods_;
}

const std::vector<std::string> &LocationInfo::GetCgi() const {
    return this->cgi_;
}

void LocationInfo::SetClientMaxBodySize(const int &client_max_body_size) {
    this->client_max_body_size_ = client_max_body_size;
}

void LocationInfo::SetPath(const std::string &path) {
    this->path_ = path;
}

void LocationInfo::SetRoot(const std::string &root) {
    this->root_ = root;
}

// setter
void LocationInfo::SetIndex(const std::string &index) {
    this->index_.push_back(index);
    // this->index_ = index;
}

void LocationInfo::SetIndex(const std::vector<std::string> &index) {
    this->index_.clear();
    for (size_t i = 0; i < index.size(); i++)
        this->index_.push_back(index[i]);
    // this->index_ = index;
}


void LocationInfo::SetAllowMethods(const std::string &allow_methods) {
    // this->allow_methods_.clear();
    this->allow_methods_.push_back(allow_methods);
}
void LocationInfo::SetAllowMethods(const std::vector<std::string> &allow_methods) {
    this->allow_methods_.clear();
    for (size_t i = 0; i < allow_methods.size(); i++)
    {
        this->allow_methods_.push_back(allow_methods[i]);
    }
}

void LocationInfo::SetCgi(const std::string &cgi) {
    this->cgi_.push_back(cgi);
}

void LocationInfo::SetCgi(const std::vector<std::string> &cgi) {
    this->cgi_ = cgi;
}

// is function
bool LocationInfo::IsCgi() const {
    if (this->cgi_.size() <= 0) 
        return false;
    return true;
}

bool LocationInfo::IsIndex() const{
    if (this->index_.size() <= 0)
        return false;
    return true;
}

void LocationInfo::print() const {
	std::cout << C_NOFAINT << "   [Location Info]   " << C_FAINT << std::endl;
	std::cout << "      path : " << GetPath() << std::endl;
	std::cout << "      root : " << GetRoot() << std::endl;
	std::cout << "      client_max_body_size : "
			  << GetClientMaxBodySize() << std::endl;
	std::cout << "      index : ";
	for (size_t i = 0; i < GetIndex().size(); i++)
		std::cout << GetIndex()[i] << ' ';
	std::cout << std::endl;
	std::cout << "      allow_methods : ";
	for (size_t i = 0; i < GetAllowMethods().size(); i++)
		std::cout << GetAllowMethods()[i] << ' ';
	std::cout << std::endl;
	std::cout << "      cgi : ";
	for (size_t i = 0; i < GetCgi().size(); i++)
		std::cout << GetCgi()[i] << ' ';
	std::cout << std::endl;
	std::cout << "      Iscgi : " << IsCgi();
	std::cout << std::endl;
	std::cout << "      IsIndex : " << IsIndex();
	std::cout << std::endl;
}