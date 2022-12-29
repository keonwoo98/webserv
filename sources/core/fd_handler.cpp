//
// Created by Alvin Lee on 2022/12/25.
//

#include <errno.h>
#include <fcntl.h>
#include "fd_handler.hpp"
#include "client_socket.hpp"
#include "udata.hpp"

int OpenFile(const Udata &user_data) {
	std::string resolved_uri = user_data.request_message_.GetResolvedUri();
	int fd;

    fd = open(resolved_uri.c_str(), O_RDONLY);
	if (fd < 0) {
        throw(HttpException(INTERNAL_SERVER_ERROR, "file open error"));
    }
	return fd;
}

