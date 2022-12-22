#include "location_info.hpp"
#include "character_color.hpp"
#include <sstream>

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
	for (size_t i = 0; i < allow_methods.size(); i++) {
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

bool LocationInfo::IsIndex() const {
	if (this->index_.size() <= 0)
		return false;
	return true;
}

std::string LocationInfo::ToString() const {
	std::stringstream ss;

	ss << C_NOFAINT << "   [Location Info]   " << C_FAINT << '\n';
	ss << "      path : " << path_ << '\n';
	ss << "      root : " << root_ << '\n';
	ss << "      client_max_body_size : " << client_max_body_size_ << '\n';
	ss << "      index : ";
	for (size_t i = 0; i < index_.size(); i++)
		ss << index_[i] << ' ';
	ss << "\n      allow_methods : ";
	for (size_t i = 0; i < allow_methods_.size(); i++)
		ss << allow_methods_[i] << ' ';
	ss << "\n      cgi : ";
	for (size_t i = 0; i < cgi_.size(); i++) {
		ss << cgi_[i] << ' ';
	}
	ss << "\n      Iscgi : " << IsCgi() << '\n';
	ss << "      IsIndex : " << IsIndex();
	return ss.str();
}

std::ostream &operator<<(std::ostream &out, const LocationInfo &location) {
	out << location.ToString();
	return out;
}
