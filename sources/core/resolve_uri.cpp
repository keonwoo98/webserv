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

ResolveURI::ResolveURI(const ServerInfo &server_info, RequestMessage &request) : server_info_(server_info),
                                                                                 request_(request),
                                                                                 base_((std::string &) ""),
                                                                                 indexes_(server_info.GetIndex()),
                                                                                 is_auto_index_(
                                                                                         server_info.IsAutoIndex()) {
    Run();
}

ResolveURI::~ResolveURI() {}

void ResolveURI::Run() {
    // root + location path + uri
	std::string decoded_uri(Decode_URI(request_.GetUri()));
    base_ = server_info_.GetRoot() + decoded_uri;
    if (decoded_uri.compare(server_info_.GetPath()) == 0 && server_info_.IsIndex() && CheckIndex()) {
        is_cgi_ = false;
        is_auto_index_ = false;
    } else if (CheckDirectory()) { // check auto index
        is_cgi_ = false;
        is_auto_index_ = true;
    } else if (server_info_.IsCgi() && CheckCGI()) { // check cgi
        is_cgi_ = true;
        is_auto_index_ = false;
    } else if (CheckStatic()) { // static
        is_cgi_ = false;
        is_auto_index_ = false;
    }
    request_.SetResolvedUri(base_);
    request_.SetQuery(cgi_query_);
}

int ResolveURI::CheckFilePermissions(std::string path) {
    if (access(path.c_str(), F_OK) != 0) {
        return NOT_FOUND;
    }
    if (access(path.c_str(), R_OK) != 0) {
        return FORBIDDEN;
    }
    return OK;
}

bool ResolveURI::CheckIndex() {
    for (std::vector<std::string>::iterator it = indexes_.begin(); it != indexes_.end(); ++it) {
        int error = CheckFilePermissions(base_ + "/" + *it);
        if (error == NOT_FOUND) {
            if (is_auto_index_) {
                if (it == std::prev(indexes_.end())) // index가 마지막 까지 없는데 auto index였다면 auto index로 다시 가야함
                    return false;
            } else {
                throw HttpException(NOT_FOUND, "file not exist");
            }
        } else if (error == FORBIDDEN) {
            throw HttpException(FORBIDDEN, "has no permision");
        } else {
            base_.append("/" + *it);
            return true;
        }
    }
    return true;
}

bool ResolveURI::CheckDirectory() {
    DIR *dir = opendir(base_.c_str());
    if (dir == NULL) { // is not a directory
        return false;
    }
    closedir(dir);
    if (!is_auto_index_) {
        throw (HttpException(FORBIDDEN, "has no permision"));
    }
    return true;
}

bool ResolveURI::CheckStatic() {
    int error = CheckFilePermissions(base_);
    if (error == NOT_FOUND) {
        throw (HttpException(NOT_FOUND, "file not exist"));
    } else if (error == FORBIDDEN) {
        throw (HttpException(FORBIDDEN, "has no permision"));
    } else {
        return true;
    }
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

bool ResolveURI::CheckCGI() {
    SplitByQuestion(base_, cgi_query_);
    int error = CheckFilePermissions(base_);
    if (error == NOT_FOUND) {
        throw (HttpException(NOT_FOUND, "file not exist"));
    } else if (error == FORBIDDEN) {
        throw (HttpException(FORBIDDEN, "has no permision"));
    } else {
        cgi_path_ = server_info_.GetCgi().at(1);
        return (FindFileExtension(base_, server_info_.GetCgi().at(0)) ? true : false);
    }
}

std::string ResolveURI::GetResolvedUri() const {
    return base_;
}

bool ResolveURI::IsAutoIndex() const {
    return is_auto_index_;
}

bool ResolveURI::IsCgi() const {
    return is_cgi_;
}

const std::string &ResolveURI::GetCgiQuery() const {
    return cgi_query_;
}

const std::string &ResolveURI::GetCgiPath() const {
    return cgi_path_;
}


std::string Decode_URI(const std::string &encoded_uri) {
	std::string decoded_uri;
	for (size_t i = 0 ; i < encoded_uri.length() ; i++) {
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
