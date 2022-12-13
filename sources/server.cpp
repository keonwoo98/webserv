#include "server.hpp"

Server::Server(): client_max_body_size_(1000000), autoindex_(false), server_name_("") {}

Server::~Server() {}

// // 얘들 디폴트 값 생각 
// 	int client_max_body_size_;
// 	bool autoindex_;
// 	std::string host_; //? 
// 	std::string port_; // ?
// 	std::string root_; // default html 
// 	std::string server_name_;
// 	std::vector<std::string> index_;
// 	std::vector<std::string> allow_methods_; // ? 
// 	std::map<int, std::string> error_pages_;
// 	std::vector<Location> locations_;
