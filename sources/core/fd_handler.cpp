//
// Created by Alvin Lee on 2022/12/25.
//

#include <errno.h>
#include <fcntl.h>
#include "fd_handler.hpp"
#include "client_socket.hpp"
#include "udata.hpp"

int OpenFile(const Udata *user_data) {
	int fd = open(user_data->request_message_.GetResolvedUri().c_str(), O_RDONLY);
    if (fd < 0) {
        if (errno == ENOENT) {
            std::perror("open: NOT_FOUND");
            throw (HTTP_EXCEPTION(NOT_FOUND));
        }
        if (errno == EACCES) {
            std::perror("open: FORBIDDEN");
            throw (HTTP_EXCEPTION(FORBIDDEN));
        }
    }
	return fd;
}

