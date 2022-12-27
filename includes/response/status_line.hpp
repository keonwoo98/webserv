//
// Created by 김민준 on 2022/12/19.
//

#ifndef WEBSERV_SOURCES_STATUS_LINE_HPP
#define WEBSERV_SOURCES_STATUS_LINE_HPP

#include <iostream>
#include <string>

#include "http_version.hpp"

class StatusLine {
   public:
	StatusLine();
	StatusLine(const HttpVersion &http_version, int status_code,
			   const std::string &reason_phrase);

	std::string ToString();
	void Clear();

   private:
	HttpVersion http_version_;
	int status_code_;
	std::string reason_phrase_;
};

std::ostream &operator<<(std::ostream &out, StatusLine status_line);

#endif	// WEBSERV_SOURCES_STATUS_LINE_H_
