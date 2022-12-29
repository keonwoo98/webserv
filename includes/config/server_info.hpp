#ifndef SERVER_INFO_HPP
#define SERVER_INFO_HPP

#include "character_color.hpp"
#include "location_info.hpp"

#define PORT_MAX 65535

class ServerInfo {
   public:
	ServerInfo();
	~ServerInfo();

	// getter
	const bool &GetAutoindex() const;
	const std::string &GetHost() const;
	const std::string &GetPort() const;
	const std::string &GetHostPort() const;
	const std::string &GetRoot() const;
	const std::vector<std::string> &GetServerName() const;
	const std::vector<std::string> &GetIndex() const;
	const std::map<int, std::string> &GetErrorPages() const;
	const std::vector<LocationInfo> &GetLocations() const;
	const static std::string &GetErrorLog();
	const std::vector<std::string> &GetCgi() const;

	// setter
	void SetClientMaxBodySize(int x);
	void SetAutoindex(const bool &x);
	void SetHost(const std::string &x);
	void SetPort(const std::string &x);
	void SetHostPort();
	void SetRoot(const std::string &x);
	void SetServerName(const std::vector<std::string> &x);
	void SetServerName(const std::string &x);

	void SetIndex(std::string &x);
	void SetErrorPages(std::string &x);

	void SetLocations(const std::vector<LocationInfo> &x);
	void SetLocations(const LocationInfo &x);

	void SetCgi(const std::vector<std::string> &x);
	void SetCgi(const std::string &x);

	void SetLocationIndex(int x);
	// is function
	bool IsIndex() const;
	bool IsErrorPages() const;
	bool IsServerName() const;
	bool IsRoot() const;
	bool IsCgi() const;
	
	std::string ToString() const;
	std::vector<std::string> GetAllowedMethod() const;
	size_t GetClientMaxBodySize() const;

   private:
	int location_index_;
	int client_max_body_size_;
	int location_index_;
	bool autoindex_;
	std::string host_;
	std::string port_;
	std::string host_port_;
	std::string root_;
	std::vector<std::string> server_name_;
	std::vector<std::string> cgi_;

	Index index_;
	ErrorPages error_pages_;

	std::vector<LocationInfo> locations_;
	const static std::string error_log_;
};

std::ostream &operator<<(std::ostream &out, const ServerInfo &server);

#endif
