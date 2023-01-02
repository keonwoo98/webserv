#ifndef LOCATION_INFO_HPP
#define LOCATION_INFO_HPP

#include <map>
#include <string>
#include <vector>

#include "error_pages.hpp"
#include "index.hpp"
#include "allow_methods.hpp"
class LocationInfo {
   public:
	LocationInfo();
	~LocationInfo();

	int GetClientMaxBodySize() const;
	bool GetAutoindex() const;
	const std::string &GetPath() const;
    const std::string GetPathNoSlash() const;
	const std::string &GetRoot() const;
	const std::vector<std::string> &GetIndex() const;
	const std::string GetErrorPagePath(int status_code);
	const std::vector<std::string> &GetAllowMethods() const;
	const std::vector<std::string> &GetCgi() const;
	const std::string &GetRedirect() const;

	void SetClientMaxBodySize(int x);
	void SetPath(const std::string &x);
	void SetRoot(const std::string &x);
	void SetIndex(std::string &x);
	void SetErrorPages(std::string &x);
	void SetAllowMethods(std::string &x);
	void SetCgi(const std::vector<std::string> &x);
	void SetCgi(const std::string &x);
	void SetAutoindex(const bool &x);
	void SetRedirect(const std::string &x);

	bool IsCgi() const;
	bool IsIndex() const;
	bool IsRoot() const;
	bool IsErrorPages() const;
	bool IsRedirect() const;
	bool IsAllowMethod(const std::string &x) const;
	std::string ToString() const;

   private:
	int client_max_body_size_;
	bool autoindex_;

	std::string path_;
	std::string root_;
	Index index_;
	ErrorPages error_pages_;
	AllowMethods allow_methods_;
	std::vector<std::string> cgi_;
	std::string redirect_;
};

std::ostream &operator<<(std::ostream &out, const LocationInfo &location);

#endif
