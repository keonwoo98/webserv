//
// Created by Alvin Lee on 2022/12/23.
//

#ifndef WEBSERV_RESOLVE_URI_HPP
#define WEBSERV_RESOLVE_URI_HPP

#include <string>

#include "server_info.hpp"

std::vector<std::string> Resolve_URI(std::string uri, ServerInfo &server_infos, int location_idx);

#endif //WEBSERV_RESOLVE_URI_HPP
