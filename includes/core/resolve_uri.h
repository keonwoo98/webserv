//
// Created by Alvin Lee on 2022/12/23.
//

#ifndef WEBSERV_RESOLVE_URI_HPP
#define WEBSERV_RESOLVE_URI_HPP

#include <string>

#include "client_socket.hpp"
#include "request_message.hpp"
#include "server_info.hpp"

void Resolve_URI(const ClientSocket &clientSocket,
				 const RequestMessage &request, Udata *udata);

#endif	// WEBSERV_RESOLVE_URI_HPP
