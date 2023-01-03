//
// Created by 김민준 on 2023/01/03.
//

#ifndef WEBSERV_INCLUDES_CORE_AUTO_INDEX_HPP_
#define WEBSERV_INCLUDES_CORE_AUTO_INDEX_HPP_

#include <string>

std::string AutoIndexHtml(const std::string &path, const std::string &dir_list);
std::string MakeDirList(const std::string &dir_uri);

#endif //WEBSERV_INCLUDES_CORE_AUTO_INDEX_HPP_
