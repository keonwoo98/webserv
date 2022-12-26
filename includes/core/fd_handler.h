//
// Created by Alvin Lee on 2022/12/25.
//

#ifndef WEBSERV_FD_HANDLER_H
#define WEBSERV_FD_HANDLER_H

#include "fcntl.h"
#include "client_socket.hpp"
#include "udata.h"

int OpenFile(const Udata &user_data);


#endif //WEBSERV_FD_HANDLER_H
