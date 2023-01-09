//
// Created by 김민준 on 2022/12/27.
//
#include <sys/event.h>
#include <unistd.h>
#include <sstream>

#include "logger.hpp"

Logger::Logger(const std::string &message) : message_(message) {
}

void Logger::WriteLog(const struct kevent &event) const {
	int ret = write((int)event.ident, message_.c_str(), message_.length());
	if (ret <= 0)
		return ;
}

std::string Logger::MakeAcceptFailLog(const ServerSocket *server_socket) {
	std::stringstream ss;

	ss << server_socket << '\n' << "Accept Failed" << std::endl;
	return ss.str();
}

std::string Logger::MakeAcceptLog(const ClientSocket *client_socket) {
	std::stringstream ss;

	ss << "New Client Accepted" << client_socket << std::endl;
	return ss.str();
}

std::string Logger::MakeRequestLog(const RequestMessage &request_message) {
	std::stringstream  ss;

	ss << request_message << std::endl;
	return ss.str();
}

