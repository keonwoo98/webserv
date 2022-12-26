#ifndef LOCATION_INFO_HPP
#define LOCATION_INFO_HPP

#include <map>
#include <string>
#include <vector>

#include "error_pages.hpp"
#include "index.hpp"

class LocationInfo {
   public:
	LocationInfo();
	~LocationInfo();

	const int &GetClientMaxBodySize() const;
	const std::string &GetPath() const;
	const std::string &GetRoot() const;
	const std::vector<std::string> &GetIndex() const;
	const std::map<int, std::string> &GetErrorPages() const;
	const std::vector<std::string> &GetAllowMethods() const;
	const std::vector<std::string> &GetCgi() const;

	void SetClientMaxBodySize(const int &x);
	void SetPath(const std::string &x);
	void SetRoot(const std::string &x);
	void SetIndex(std::string &x);
	void SetErrorPages(std::string &x);
	void SetAllowMethods(const std::string &x);
	void SetAllowMethods(const std::vector<std::string> &x);
	void SetCgi(const std::vector<std::string> &x);
	void SetCgi(const std::string &x);

	bool IsCgi() const;
	bool IsIndex() const;
	bool IsRoot() const;

	std::string ToString() const;

   private:
	int client_max_body_size_;
	std::string path_;
	std::string root_;
	Index index_;
	ErrorPages error_pages_;
	std::vector<std::string> allow_methods_;
	std::vector<std::string> cgi_;
};

std::ostream &operator<<(std::ostream &out, const LocationInfo &location);

#endif
