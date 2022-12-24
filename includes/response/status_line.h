//
// Created by 김민준 on 2022/12/19.
//

#ifndef WEBSERV_SOURCES_STATUS_LINE_H_
#define WEBSERV_SOURCES_STATUS_LINE_H_

#include "http_version.h"

#include <string>
#include <iostream>

class StatusLine {
   private:
	HttpVersion http_version_;
	int status_code_;
	std::string reason_phrase_;
   public:
	StatusLine();
	StatusLine(const HttpVersion &http_version, int status_code, const std::string &reason_phrase);
	std::string ToString();
	void Clear();
};

std::ostream &operator<<(std::ostream &out, StatusLine status_line);

#endif //WEBSERV_SOURCES_STATUS_LINE_H_
