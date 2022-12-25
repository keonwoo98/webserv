//
// Created by Alvin Lee on 2022/12/25.
//

#include "fd_handler.h
#include "client_socket.hpp"
#include "fcntl.h"

int OpenFile(ClientSocket &client) {
    int fd = open(client.GetResolvedUri(), O_RDONLY);
    return fd;
}