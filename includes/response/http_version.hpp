//
// Created by 김민준 on 2022/12/19.
//

#ifndef WEBSERV_SOURCES_HTTP_VERSION_HPP
#define WEBSERV_SOURCES_HTTP_VERSION_HPP

#include <string>

class HttpVersion {
   private:
	int major_;
	int minor_;

   public:
	explicit HttpVersion(int major = 1, int minor = 1);
	std::string ToString() const;
};

std::ostream &operator<<(std::ostream &out, HttpVersion http_version);

#endif	// WEBSERV_SOURCES_HTTP_VERSION_H_
