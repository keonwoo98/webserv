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
    ResolveURI(const ServerInfo &server_info, RequestMessage &request);
    ~ResolveURI();

    void Run();

    int CheckFilePermissions(std::string path);
    bool CheckIndex();
    bool CheckDirectory();
    bool CheckCGI();
private:
    const ServerInfo &server_info_;
    RequestMessage &request_;

    std::string &base_;

    std::vector<std::string> indexes_;

    bool is_auto_index_;
    bool is_cgi_;
    std::string cgi_query_;
    std::string cgi_path_;
};

//void Resolve_URI(const ClientSocket *clientSocket, Udata *user_data);

#endif	// WEBSERV_RESOLVE_URI_HPP
