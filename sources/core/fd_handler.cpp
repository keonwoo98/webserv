//
// Created by Alvin Lee on 2022/12/25.
//

#include <fcntl.h>
#include <cerrno>
#include "fd_handler.hpp"
#include "http_exception.hpp"
#include "udata.hpp"

int OpenFile(const Udata *user_data) {
	int fd = open(user_data->request_message_.GetResolvedUri().c_str(), O_RDONLY);
    if (fd < 0) {
        if (errno == ENOENT) {
            throw HttpException(NOT_FOUND, "OpenFile open() NOT_FOUND");
        }
        if (errno == EACCES) {
            throw HttpException(FORBIDDEN, "OpenFile open() FORBIDDEN");
        }
    }
	return fd;
}

