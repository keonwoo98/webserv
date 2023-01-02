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

    bool CheckStatic();

    std::string GetResolvedUri() const;

    bool IsAutoIndex() const;

    bool IsCgi() const;

    const std::string &GetCgiQuery() const;

    const std::string &GetCgiPath() const;

private:
    ServerInfo server_info_;
    RequestMessage &request_;

    std::string base_;

    std::vector<std::string> indexes_;

    bool is_auto_index_;
    bool is_cgi_;
    std::string cgi_query_;
    std::string cgi_path_;
};

std::string Decode_URI(const std::string &encoded_uri);

//void Resolve_URI(const ClientSocket *clientSocket, Udata *user_data);

#endif    // WEBSERV_RESOLVE_URI_HPP
