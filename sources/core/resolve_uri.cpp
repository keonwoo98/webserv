//
// Created by Alvin Lee on 2022/12/23.
//

#include "resolve_uri.hpp"
#include "client_socket.hpp"
#include "http_exception.hpp"
#include "request_parser.hpp"

#include <unistd.h>
#include <iostream>
#include <dirent.h>

std::string ResolveURI::ResolveAlias(std::string uri) {
	if (server_info_.IsAlias()) {
		return uri.substr(server_info_.GetPath().length());
	} else {
		return uri;
	}
}

ResolveURI::ResolveURI(const ServerInfo &server_info, RequestMessage &request) : server_info_(server_info),
																				 request_(request),
																				 base_((std::string &) ""),
																				 indexes_(server_info.GetIndex()),
																				 is_auto_index_(
																						 server_info.IsAutoIndex()),
																				 is_cgi_(server_info.IsCgi()) {
	base_ = server_info_.GetRoot() + ResolveAlias(Decode_URI(request_.GetUri()));
}

ResolveURI::~ResolveURI() {}


// checking directory
bool ResolveURI::CheckDirectory(std::string uri) {
	DIR *dir = opendir(uri.c_str());
	if (dir == NULL) { // is not a directory
		return false;
	}
	closedir(dir);
	return true;
}

//std::string location_path(server_info_.GetRoot() + server_info_.GetPath());
//	if (uri.compare(0, location_path.length(), location_path) == 0) {
//		return true;
//	}
// index checking
bool ResolveURI::CheckIndex(std::string uri) {
	std::string path(server_info_.GetRoot() + ResolveAlias(server_info_.GetPath()));
	if (!server_info_.IsIndex()) {
		indexes_.push_back("index.html");
	}
	if (uri.compare(0, path.length(), path) == 0 && CheckDirectory(uri)) {
		return true;
	}
	return false;
}

// if directory() -> index check and auto index check
// index checking function
// auto index checking function

int ResolveURI::CheckFilePermissions(std::string path) {
	if (access(path.c_str(), F_OK) != 0) {
		return NOT_FOUND;
	}
	if (access(path.c_str(), R_OK) != 0) {
		return FORBIDDEN;
	}
	return OK;
}

bool ResolveURI::ResolveIndex() { // return true : auto index | return : false auto index X
	if (CheckIndex(base_)) {
		std::string appended_uri;
		for (std::vector<std::string>::iterator it = indexes_.begin(); it != indexes_.end(); ++it) {
			if (base_.at(base_.length() - 1) == '/') {
				base_.erase(0, base_.length() - 1);
			}
			appended_uri = base_ + "/" + *it;
			int error = CheckFilePermissions(appended_uri);
			if (error == NOT_FOUND) {
				if (is_auto_index_) {
					if (it == --indexes_.end())
						return true;
				} else {
					base_ = appended_uri;
					return false;
				}
			} else if (error == FORBIDDEN) {
				base_ = appended_uri;
				return false;
			} else {
				base_ = appended_uri;
				return false;
			}
		}
	} else if (is_auto_index_ && CheckDirectory(base_)) {
		return true;
	}
	return false;
}

bool ResolveURI::ResolveCGI() {
	if (is_cgi_) {
		SplitByQuestion(base_, cgi_query_);
		cgi_path_ = server_info_.GetCgi().at(1);
		request_.SetQuery(cgi_query_);
//        request_.SetResolvedUri(base_);
		return (FindFileExtension(base_, server_info_.GetCgi().at(0)) ? true : false);
	}
	return false;
}

std::string ResolveURI::GetResolvedUri() const {
	return base_;
}

const std::string &ResolveURI::GetCgiPath() const {
	return cgi_path_;
}

void SplitByQuestion(std::string &uri, std::string &cgi_query_string) {
	std::size_t pos = uri.rfind("?");
	if (pos == std::string::npos) {
		return;
	}
	cgi_query_string = uri.substr(pos + 1);
	uri = uri.substr(0, pos);
}

bool FindFileExtension(std::string uri, std::string file_extension) {
	std::size_t pos = uri.rfind(file_extension);
	if (pos != std::string::npos) {
		if (uri.length() - file_extension.length() == pos) {
			return true;
		} else {
			return false;
		}
	}
	return false;
}

std::string Decode_URI(const std::string &encoded_uri) {
	std::string decoded_uri;
	for (size_t i = 0; i < encoded_uri.length(); i++) {
		char c = encoded_uri[i];
		if (c == '%') {
			if (i + 2 > encoded_uri.length())
				throw HttpException(BAD_REQUEST, "invalid uri");
			std::string hex(encoded_uri.substr(i + 1, 2));
			i += 2;
			c = static_cast<char>(hexstrToDec(hex));
		}
		decoded_uri.push_back(c);
	}
	return decoded_uri;
}
