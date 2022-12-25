//
// Created by Alvin Lee on 2022/12/25.
//

#include <errno.h>
#include <fcntl.h>
#include "fd_handler.h
#include "client_socket.hpp"
#include "handle_event_exception.h"
#include "udata.h"

int OpenFile(ClientSocket &client, Udata &user_data) {
    int fd = open(client.GetResolvedUri(), O_RDONLY);
    if (fd < 0) {
        if (errno == ENOENT) {
            std::perror("open: NOT_FOUND");
            user_data.request_message_.SetStatusCode(NOT_FOUND);
            throw (HandleEventExeption::OpenExeption());
        }
        if (errno == EACCES) {
            std::perror("open: FORBIDDEN");
            user_data.request_message_.SetStatusCode(FORBIDDEN);
            throw (HandleEventExeption::OpenExeption());
        }
    }
    return fd;
}

