//
// Created by Alvin Lee on 2022/12/23.
//

#include "resolve_uri.hpp"
#include "client_socket.hpp"
#include "http_exception.hpp"

#include <unistd.h>
#include <iostream>
#include <dirent.h>


#define DEFAULT_ROOT "/var/www"
#define NOT_INDEX_URI 0
#define LOCATION_INDEX_URI 1
#define SERVER_INDEX_URI 2


std::string GetServerBaseURI(ServerInfo &server_info) {
    std::string base_uri;
    if (server_info.IsRoot()) { // server block에 root가 존재 할때
        base_uri.append(server_info.GetRoot());
    } else { // server block에 root가 존재 하지 않을때
        base_uri.append(DEFAULT_ROOT);
    }
    return base_uri;
}

std::string GetLocationBaseURI(ServerInfo &server_info, LocationInfo &location_info) {
    std::string base_uri;

    if (location_info.IsRoot()) { // location block에 root가 존재 할떄
        base_uri.append(location_info.GetRoot());
    } else { // location block에 root가 존재하지 않을때
        if (server_info.IsRoot()) { // server에 root가 존재했더라면,
            base_uri.append(server_info.GetRoot());
        } else { // server에도 root가 없다
            base_uri.append(DEFAULT_ROOT);
        }
    }
    return base_uri;
}

void AppendSlashIfNotExist(std::string &uri) {
    if (uri.back() != '/') { // uri / 추가로 맞춰주기
        uri.append("/");
    }
}

int IsIndexURI(std::string requested_uri, ServerInfo &server_info, LocationInfo &location_info) {
    AppendSlashIfNotExist(requested_uri);
    if (location_info.IsIndex()) { // location index 존재하는 경우
        std::string path(location_info.GetPath());
        AppendSlashIfNotExist(path);
        if (requested_uri.compare(path) == 0) { // (location: /a/ , uri /a/) 경우
            return LOCATION_INDEX_URI;
        } else { // uri 다름
            return NOT_INDEX_URI;
        }
    } else if (server_info.IsIndex() && requested_uri.compare("/") == 0) { // server index 존재하는 경우
        return SERVER_INDEX_URI; // (location: /, uri: / ) 인 경우
    } else { // index 없음
        return NOT_INDEX_URI;
    }
}

std::vector<std::string> AppendIndexPathToBaseURI(std::vector<std::string> index_pathes, std::string base_uri) {
    std::vector<std::string> resolved_uri;
    for (std::vector<std::string>::iterator it = index_pathes.begin(); it != index_pathes.end(); ++it) {
        resolved_uri.push_back(base_uri + '/' + *it);
    }
    return resolved_uri;
}

std::string CheckFileExist(std::vector<std::string> uri, bool is_index, bool &is_auto_index) {
    for (std::vector<std::string>::iterator it = uri.begin(); it != uri.end(); ++it) {
        if (access(it->c_str(), F_OK) != 0) {
            if (is_index && is_auto_index) {
                if (it == --uri.end()) {
                    return (*it);
                }
            }
            else {
                throw (HttpException(NOT_FOUND, "file not exist"));
            }
        } else if (access(it->c_str(), R_OK) != 0) {
            throw (HttpException(FORBIDDEN, "has no permission"));
        } else {
            if (is_auto_index) {
                if (!is_index) {
                    DIR *dir = opendir(it->c_str());
                    if (dir == NULL) { // is not a directory
                        is_auto_index = false;
                    } else {
                        closedir(dir);
                    }
                } else {
                    is_auto_index = false;
                }
            }
            return (*it);
        }
    }
    return ("");
}

bool GetAutoIndex(ServerInfo &server_info, LocationInfo &location_info, int location_idx) {
    if (location_idx != -1) {
        if (location_info.GetAutoindex()) { // location에 auto index가 존재할 경우
            return true;
        } else if (server_info.GetAutoindex()) { // server에 auto index가 존재할 경우
            return true;
        }
        return false;
    } else {
        if (server_info.GetAutoindex()) {
            return true;
        } else {
            return false;
        }
    }
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

void FindQuestionIndex(std::string &uri, std::string &cgi_query_string) {
    std::size_t pos = uri.rfind("?");
    if (pos == std::string::npos) {
        return ;
    }
    cgi_query_string = uri.substr(pos + 1);
    uri = uri.substr(0, pos);
}

void
CheckCGI(std::string &uri, ServerInfo &server_info, LocationInfo &location_info, bool &is_cgi, std::string &cgi_path, std::string &cgi_query_string) {
    std::string file_extension;
    (void)server_info; // temp

    if (location_info.IsCgi()) { // location에 cgi 가 존재할 경우
        file_extension = location_info.GetCgi().at(0);
        cgi_path = location_info.GetCgi().at(1);
        FindQuestionIndex(uri, cgi_query_string);
        is_cgi = FindFileExtension(uri, file_extension);
    }
//    else if (server_info.IsCGI()) { // server에 cgi 가 존재할 경우 server_info.IsCGI not implement
//        file_extension = server_info.GetCgi().at(0);
//        cgi_path = server_info.GetCgi().at(1);
//        FindQuestionIndex(uri, cgi_query_string);
//        is_cgi = FindFileExtension(uri, file_extension);
//        is_auto_index = false;
//    }
    else { // 둘다 존재하지 않을경우
        is_cgi = false;
    }
}

void Resolve_URI(const ClientSocket &client, Udata *user_data) {
    RequestMessage &request = user_data->request_message_;
    std::string requested_uri = request.GetUri();
    ServerInfo server_info = client.GetServerInfo();
    int location_idx = client.GetLocationIndex();

    std::vector<std::string> index_pathes;
    std::vector<std::string> resolved_uri_vec;
    std::string resolved_uri;
    LocationInfo location_info;
    std::string base_uri;

    bool is_auto_index;
    bool is_cgi = false;
    std::string cgi_path;
    std::string cgi_query_string;

    if (location_idx != -1) {
        location_info = server_info.GetLocations().at(location_idx);
    }
    is_auto_index = GetAutoIndex(server_info, location_info, location_idx);
    CheckCGI(requested_uri, server_info, location_info, is_cgi, cgi_path, cgi_query_string);
    // TODO: index 확인 작업
    if (location_idx == -1) { // server block만 있는 경우
        base_uri = GetServerBaseURI(server_info);
        if (server_info.IsIndex() && requested_uri.compare("/") == 0) { // index를 추가해줘야함
            index_pathes = server_info.GetIndex();
            resolved_uri_vec = AppendIndexPathToBaseURI(index_pathes, base_uri);
            resolved_uri = CheckFileExist(resolved_uri_vec, true, is_auto_index);
        } else {
            resolved_uri_vec.push_back(base_uri + requested_uri);
            resolved_uri = CheckFileExist(resolved_uri_vec, false, is_auto_index);
        }
    } else { // server block + location block 이 있는 경우
        base_uri = GetLocationBaseURI(server_info, location_info);
        int ret = IsIndexURI(requested_uri, server_info, location_info);
        if (ret == NOT_INDEX_URI) { // index가 없는 경우
            resolved_uri_vec.push_back(base_uri + requested_uri);
            resolved_uri = CheckFileExist(resolved_uri_vec, false, is_auto_index);
        } else if (ret == LOCATION_INDEX_URI) { // location index값으로 추가해줘야함
            index_pathes = location_info.GetIndex();
            resolved_uri_vec = AppendIndexPathToBaseURI(index_pathes, base_uri + requested_uri);
            resolved_uri = CheckFileExist(resolved_uri_vec, true, is_auto_index);
        } else if (ret == SERVER_INDEX_URI) {
            index_pathes = server_info.GetIndex();
            resolved_uri_vec = AppendIndexPathToBaseURI(index_pathes, base_uri + requested_uri);
            resolved_uri = CheckFileExist(resolved_uri_vec, true, is_auto_index);
        }
    }
    // location.GetPath()의 / 가 없으면 추가하고 ( || 로 size -1 과 size 일때 uri와 비교)
    // TODO: auto index, cgi 해결
    // TODO: file 존재 확인
    request.SetResolvedUri(resolved_uri);
    request.SetCgiQuery(cgi_query_string);
    request.SetCgiExePath(cgi_path);
    request.SetIsAutoIndex(is_auto_index);
    request.SetIsCgi(is_cgi);
//
//	if (server_infos.IsRoot()) { // root 존재
//		base_uri.append(server_infos.GetRoot());
//	} else // root 존재 안함
//		base_uri.append(default_root);
//	if (location_idx == -1) {
//		if (server_infos.IsIndex() && uri.compare("/") == 0) { // index 존재 하는 경우 and uri '/'
//			index = server_infos.GetIndex();
//			for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); ++it) {
//				resolved_uri.push_back(base_uri + '/' + *it);
//			}
//		} else { // index 존재 하지 않는 경우
//			base_uri.append(uri);
//			resolved_uri.push_back(base_uri);
//		}
//	} else {
//		location_info = server_infos.GetLocations().at(location_idx);
//		if (location_info.IsRoot()) { // root 존재
//			base_uri.clear();
//			base_uri.append(location_info.GetRoot());
//		}
//		if (location_info.IsIndex() &&
//			uri.compare(location_info.GetPath()) == 0) { // index 존재 하는 경우 and uri '/location'
//			base_uri.append(location_info.GetPath());
//			index = location_info.GetIndex();
//			for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); ++it) {
//				resolved_uri.push_back(base_uri + '/' + *it);
//			}
//		} else {
//			base_uri.append(uri);
//			resolved_uri.push_back(base_uri);
//		}
//	}
//	user_data->request_message_.SetResolvedUri(resolved_uri);
}
