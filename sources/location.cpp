#include "location.hpp"

Location::Location() : client_max_body_size_(1000000), path_(""), root_("./docs/index.html") {}

Location::~Location() {}

const int &Location::GetClientMaxBodySize() const {
    return this->client_max_body_size_;
}

const std::string &Location::GetPath() const {
    return this->path_;
}

const std::string &Location::GetRoot() const {
    return this->root_;
}

const std::vector<std::string> &Location::GetIndex() const {
    return this->index_;
}

const std::vector<std::string> &Location::GetAllowMethods() const {
    return this->allow_methods_;
}

const std::vector<std::string> &Location::GetCgi() const {
    return this->cgi_;
}

void Location::SetClientMaxBodySize(const int &client_max_body_size) {
    this->client_max_body_size_ = client_max_body_size;
}

void Location::SetPath(const std::string &path) {
    this->path_ = path;
}

void Location::SetRoot(const std::string &root) {
    this->root_ = root;
}

void Location::SetIndex(const std::string &index) {
    this->index_.push_back(index);
    // this->index_ = index;
}

void Location::SetIndex(const std::vector<std::string> &index) {
    this->index_.clear();
    for (size_t i = 0; i < index.size(); i++)
        this->index_.push_back(index[i]);
    // this->index_ = index;
}


void Location::SetAllowMethods(const std::string &allow_methods) {
    // this->allow_methods_.clear();
    this->allow_methods_.push_back(allow_methods);
}
void Location::SetAllowMethods(const std::vector<std::string> &allow_methods) {
    this->allow_methods_.clear();
    for (size_t i = 0; i < allow_methods.size(); i++)
    {
        this->allow_methods_.push_back(allow_methods[i]);
    }
}

void Location::SetCgi(const std::string &cgi) {
    this->cgi_.push_back(cgi);
}

void Location::SetCgi(const std::vector<std::string> &cgi) {
    this->cgi_ = cgi;
}
