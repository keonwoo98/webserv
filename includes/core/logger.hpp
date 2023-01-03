//
// Created by 김민준 on 2022/12/27.
//
#ifndef WEBSERV_INCLUDES_CORE_LOGGER_HPP_
#define WEBSERV_INCLUDES_CORE_LOGGER_HPP_

#include <string>

class Logger {
   public:
	explicit Logger(const std::string &message);
	void WriteLog(const struct kevent &event) const;
   private:
	std::string message_;
};

#endif //WEBSERV_INCLUDES_CORE_LOGGER_HPP_
