//
// Created by 김민준 on 2022/12/27.
//
#ifndef WEBSERV_INCLUDES_CORE_LOGGER_HPP_
#define WEBSERV_INCLUDES_CORE_LOGGER_HPP_

#include <string>

#include "server_socket.hpp"

class Logger {
   public:
	explicit Logger(const std::string &message);
	void WriteLog(const struct kevent &event) const;
	static std::string MakeAcceptFailLog(const ServerSocket *server_socket);
	static std::string MakeAcceptLog(const ClientSocket *client_socket);
	static std::string MakeRequestLog(const RequestMessage &request_message);
   private:
	std::string message_;
};

#endif //WEBSERV_INCLUDES_CORE_LOGGER_HPP_
