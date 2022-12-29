//
// Created by Alvin Lee on 2022/12/23.
//

#ifndef WEBSERV_RESOLVE_URI_HPP
#define WEBSERV_RESOLVE_URI_HPP

#include <string>

#include "client_socket.hpp"
#include "request_message.hpp"
#include "server_info.hpp"

class ResolveURI {
public:
    ResolveURI();
    ResolveURI(ServerInfo &server_info, int location_idx);
    ~ResolveURI();
private:
    ServerInfo serverInfo_;
    LocationInfo locationInfo_;
    int location_idx;
};

void Resolve_URI(const ClientSocket *clientSocket, Udata *user_data);

#endif	// WEBSERV_RESOLVE_URI_HPP
