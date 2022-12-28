//
// Created by 김민준 on 2022/12/27.
//
#include <sys/event.h>
#include <unistd.h>

#include "logger.hpp"

Logger::Logger(const std::string &message) : message_(message) {
}

std::string &Logger::GetMessage() {
	return message_;
}
void Logger::WriteLog(struct kevent &event) {
	write(event.ident, message_.c_str(), message_.length());
}



