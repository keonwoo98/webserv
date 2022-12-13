#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>
#include <map>

class Location {
   private:
	int client_max_body_size_;
	std::string path_;
	std::string root_;
	std::vector<std::string> index_;
	std::vector<std::string> allow_methods_;
	std::vector<std::string> cgi_;
   public:
	Location();
	~Location();
	const int &Location::GetClientMaxBodySize() const;
	const std::string &Location::GetPath() const;
	const std::string &Location::GetRoot() const;
	const std::vector<std::string> &Location::GetIndex() const;
	const std::vector<std::string> &Location::GetAllowMethods() const;
	const std::vector<std::string> &Location::GetCgi() const;
	
	void Location::SetClientMaxBodySize(const int &client_max_body_size);
	void Location::SetPath(const std::string &path);
	void Location::SetRoot(const std::string &root);
	void Location::SetIndex(const std::vector<std::string> &index);
	void Location::SetAllowMethods(const std::string &allow_methods);
	void Location::SetAllowMethods(const std::vector<std::string> &allow_methods);
	void Location::SetCgi(const std::vector<std::string> &cgi);
};

#endif