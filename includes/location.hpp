#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>
#include <map>

class Location {
   public:
	int client_max_body_size_;
	std::string path_;
	std::string root_;
	std::vector<std::string> index_;
	std::vector<std::string> allow_methods_;
	std::map<std::string, std::string> cgi_;
   public:
	Location();
	~Location();
};

#endif
