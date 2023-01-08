//
// Created by Alvin Lee on 2022/12/25.
//

#ifndef WEBSERV_FD_HANDLER_HPP
#define WEBSERV_FD_HANDLER_HPP

#include "fcntl.h"
#include "client_socket.hpp"
#include "udata.hpp"

void CheckStaticFileOpenError(int fd);
long GetFileSize(const char *file_path);

#endif //WEBSERV_FD_HANDLER_H
