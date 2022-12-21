#ifndef SERVER_HPP
#define SERVER_HPP

#include "location.hpp"

class Server {
   private:
	int client_max_body_size_;
	bool autoindex_;
	std::string host_;
	std::string port_;
	std::string host_port_;
	std::string root_;
	std::vector<std::string> server_name_; // vector 로 고쳐야함 
	std::vector<std::string> index_;
	std::vector<std::string> allow_methods_;
	std::map<int, std::string> error_pages_;
	std::vector<Location> locations_;

   public:
	Server();
	~Server();
	// getter
	const int &GetClientMaxBodySize() const;
	const bool &GetAutoindex() const;
	const std::string &GetHost() const;
	const std::string &GetPort() const;
	const std::string &GetHostPort() const;
	const std::string &GetRoot() const;
	const std::vector<std::string> &GetServerName() const;
	const std::vector<std::string> &GetIndex() const;
	const std::vector<std::string> &GetAllowMethods() const;
	std::map<int, std::string> &GetErrorPages();
	const std::vector<Location> &GetLocations() const;

	// setter
	void SetClientMaxBodySize(const int &x);
	void SetAutoindex(const bool &x);
	void SetHost(const std::string &x);
	void SetPort(const std::string &x);
	void SetHostPort();
	void SetRoot(const std::string &x);
	void SetServerName(const std::vector<std::string> &x);
	void SetServerName(const std::string &x);
	void SetIndex(const std::vector<std::string> &x);
	void SetIndex(const std::string &x);
	void SetAllowMethods(const std::vector<std::string> &x);
	void SetAllowMethods(const std::string &x);
	void SetErrorPages(const std::map<int, std::string> &x);
	void SetErrorPages(int &x, std::string &y);
	void SetErrorPages(const std::pair<int, std::string> &x);
	void SetLocations(const std::vector<Location> &x);
	void SetLocations(const Location &x);

	// is function
	bool IsIndex() const;
	bool IsErrorPages() const;
	bool IsServerName() const;
};

#endif
