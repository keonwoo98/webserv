#include "location_info.hpp"
#include "character_color.hpp"
#include <sstream>

LocationInfo::LocationInfo()
	: client_max_body_size_(-1),
	  autoindex_(false),
	  path_(""),
	  root_(""),
	  index_(),
	  redirect_(""),
	  upload_path_("") {}

LocationInfo::~LocationInfo() {}

// getter
int LocationInfo::GetClientMaxBodySize() const {
	return this->client_max_body_size_;
}

const std::string &LocationInfo::GetPath() const { return this->path_; }

const std::string &LocationInfo::GetRoot() const { return this->root_; }

const std::vector<std::string> &LocationInfo::GetIndex() const {
	return this->index_.GetIndex();
}

const std::string LocationInfo::GetPathNoSlash() const{
    std::string temp = this->GetPath();
    if (*(temp.end()-1) == '/') {
        int len = temp.length();
        temp = temp.substr(0,len -1);
    }
    return temp;
}

const std::string LocationInfo::GetErrorPagePath(int status_code) {
	return this->error_pages_.GetPath(status_code);
}

const std::vector<std::string> &LocationInfo::GetAllowMethods() const {
	return this->allow_methods_.GetAllowMethods();
}

const std::vector<std::string> &LocationInfo::GetCgi() const {
	return this->cgi_;
}
bool LocationInfo::GetAutoindex() const { return this->autoindex_; }

const std::string &LocationInfo::GetRedirect() const { return this->redirect_; }

const std::string &LocationInfo::GetAliasPath() const { return this->alias_; }

/// @Setter
void LocationInfo::SetClientMaxBodySize(int client_max_body_size) {
	this->client_max_body_size_ = client_max_body_size;
}

const std::string &LocationInfo::GetUploadPath() const { return this->upload_path_; }

void LocationInfo::SetPath(const std::string &x) { this->path_ = x; }

void LocationInfo::SetRoot(const std::string &x) { 
	if (*(x.end()-1) != '/')
		this->root_ = x;
	else {
		int len = x.length();
		this->root_ = x.substr(0,len -1);
	}
}

void LocationInfo::SetIndex(std::string &x) {
	Index index(x);
	this->index_ = index;
}

void LocationInfo::SetErrorPages(std::string &x) {
	this->error_pages_.Append(x);
}

void LocationInfo::SetAllowMethods(std::string &x) {
	this->allow_methods_.Append(x);
}

void LocationInfo::SetCgi(const std::string &x) { this->cgi_.push_back(x); }

void LocationInfo::SetCgi(const std::vector<std::string> &x) { this->cgi_ = x; }

void LocationInfo::SetAutoindex(const bool &x) { this->autoindex_ = x; }

void LocationInfo::SetRedirect(const std::string &x) { this->redirect_ = x; }

void LocationInfo::SetUploadPath(const std::string &x) { this->upload_path_ = x; }

void LocationInfo::SetAlias(const std::string &x) { this->alias_ = x; }

// is function
bool LocationInfo::IsCgi() const {
	if (this->cgi_.size() <= 0) return false;
	return true;
}

bool LocationInfo::IsIndex() const {
	if (this->index_.GetIndex().size() <= 0) return false;
	return true;
}

bool LocationInfo::IsErrorPages() const {
	if (this->error_pages_.GetErrorPages().size() <= 0) return false;
	return true;
}

bool LocationInfo::IsRoot() const {
	if (this->root_.size() <= 0) return false;
	return true;
}

bool LocationInfo::IsRedirect() const {
	if (this->redirect_.size() <= 0) return false;
	return true;
}

bool LocationInfo::IsAllowMethod(const std::string &x) const {
	return this->allow_methods_.IsAllowedMethod(x);
}
bool LocationInfo::IsAlias() const {
	if (this->alias_.size() <= 0) return false;
	return true;
}
std::string LocationInfo::ToString() const {
	std::stringstream ss;

	ss << C_NOFAINT << "   [Location Info]   " << C_FAINT << '\n';
	ss << "      path                 : " << path_ << '\n';
	ss << "      root                 : " << root_ << '\n';
	ss << "      alias                : " << alias_ << '\n';
	ss << "      autoindex            : " << autoindex_ << '\n';
	ss << "      client_max_body_size : " << client_max_body_size_ << '\n';
	ss << "      index                : ";
	ss << index_;
	ss << "\n      error_pages          : " << '\n';
	ss << error_pages_;
	ss << "\n      allow_methods        : ";
	ss << allow_methods_;
	ss << "\n      redirect             : " << redirect_;
	ss << "\n      upload_path          : " << upload_path_;
	ss << "\n      cgi                  : ";
	for (size_t i = 0; i < cgi_.size(); i++) {
		ss << cgi_[i] << ' ';
	}
	ss << "\n      Iscgi        : " << IsCgi() << '\n';
	ss << "      IsRoot       : " << IsRoot() << '\n';
	ss << "      IsErrorPages : " << IsErrorPages() << '\n';
	ss << "      IsIndex      : " << IsIndex() << '\n';
	ss << "      IsAlias      : " << IsAlias();
	return ss.str();
}

std::ostream &operator<<(std::ostream &out, const LocationInfo &location) {
	out << location.ToString();
	return out;
}
