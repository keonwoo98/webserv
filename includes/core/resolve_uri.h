//
// Created by Alvin Lee on 2022/12/23.
//

#ifndef WEBSERV_RESOLVE_URI_HPP
#define WEBSERV_RESOLVE_URI_HPP

#include <string>

#include "server_info.hpp"
#include "client_socket.hpp"
#include "request_message.hpp"

void Resolve_URI(ClientSocket &clientSocket, RequestMessage &request);

#endif //WEBSERV_RESOLVE_URI_HPP
