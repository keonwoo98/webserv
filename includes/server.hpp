#ifndef SERVER_HPP
#define SERVER_HPP

#include "location.hpp"

class Server {
   public:
	int client_max_body_size_;
	bool autoindex_;
	std::string host_;
	std::string port_;
	std::string root_;
	std::string server_name_;
	std::vector<std::string> index_;
	std::vector<std::string> allow_methods_;
	std::map<int, std::string> error_pages_;
	std::vector<Location> locations_;

   public:
	Server();
	~Server();
};

#endif
