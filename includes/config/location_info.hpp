#ifndef LOCATION_INFO_HPP
#define LOCATION_INFO_HPP

#include <string>
#include <vector>
#include <map>

class LocationInfo {
   private:
	int client_max_body_size_;
	std::string path_;
	std::string root_;
	std::vector<std::string> index_;
	std::vector<std::string> allow_methods_;
	std::vector<std::string> cgi_;
   public:
	LocationInfo();
	~LocationInfo();
	const int &GetClientMaxBodySize() const;
	const std::string &GetPath() const;
	const std::string &GetRoot() const;
	const std::vector<std::string> &GetIndex() const;
	const std::vector<std::string> &GetAllowMethods() const;
	const std::vector<std::string> &GetCgi() const;
	
	void SetClientMaxBodySize(const int &client_max_body_size);
	void SetPath(const std::string &path);
	void SetRoot(const std::string &root);
	void SetIndex(const std::string &index);
	void SetIndex(const std::vector<std::string> &index);
	void SetAllowMethods(const std::string &allow_methods);
	void SetAllowMethods(const std::vector<std::string> &allow_methods);
	void SetCgi(const std::vector<std::string> &cgi);
	void SetCgi(const std::string &cgi);

	bool IsCgi() const;
	bool IsIndex() const;

	std::string ToString() const;

};

std::ostream &operator<<(std::ostream &out, const LocationInfo &location);

#endif
