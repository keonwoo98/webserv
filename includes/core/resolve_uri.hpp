//
// Created by Alvin Lee on 2022/12/23.
//

#ifndef WEBSERV_RESOLVE_URI_HPP
#define WEBSERV_RESOLVE_URI_HPP

#include <string>

#include "client_socket.hpp"
#include "request_message.hpp"
#include "server_info.hpp"

void SplitByQuestion(std::string &uri, std::string &cgi_query_string);

bool FindFileExtension(std::string uri, std::string file_extension);

std::string Decode_URI(const std::string &encoded_uri);

class ResolveURI {
public:
	ResolveURI(const ServerInfo &server_info, RequestMessage &request);

	~ResolveURI();

	int CheckFilePermissions(std::string path);

	bool CheckIndex(std::string uri);

	bool ResolveIndex();

	bool CheckDirectory(std::string uri);

	bool ResolveCGI();

	std::string ResolveAlias(std::string uri);

	std::string GetResolvedUri() const;

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

#endif    // WEBSERV_RESOLVE_URI_HPP
