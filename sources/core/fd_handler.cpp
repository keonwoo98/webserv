//
// Created by Alvin Lee on 2022/12/25.
//

#include <errno.h>
#include <fcntl.h>
#include "fd_handler.h
#include "client_socket.hpp"
#include "handle_event_exception.h"
#include "udata.h"

int OpenFile(Udata &user_data) {
    std::vector<std::string> resolved_uri = user_data.request_message_.GetResolvedUri();
    int fd;

    for (size_t i = 0; i < resolved_uri.size(); i++) {
        fd = open(resolved_uri.at(i).c_str(), O_RDONLY);
        if (fd < 0) {
            if (errno == ENOENT && i == resolved_uri.size() - 1) {
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
    }
    return fd;
}

