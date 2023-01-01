//
// Created by 김민준 on 2022/12/27.
//
#include <sys/event.h>
#include <unistd.h>

#include "logger.hpp"

Logger::Logger(const std::string &message) : message_(message) {
}

void Logger::WriteLog(const struct kevent &event) const {
	write((int)event.ident, message_.c_str(), message_.length());
}



