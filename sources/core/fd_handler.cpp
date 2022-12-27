//
// Created by Alvin Lee on 2022/12/25.
//

#include <errno.h>
#include <fcntl.h>
#include "fd_handler.hpp"
#include "client_socket.hpp"
#include "udata.hpp"

int OpenFile(const Udata &user_data) {
	std::vector<std::string> resolved_uri = user_data.request_message_.GetResolvedUri();
	int fd;

	for (size_t i = 0; i < resolved_uri.size(); i++) {
		fd = open(resolved_uri.at(i).c_str(), O_RDONLY);
		if (fd < 0) {
			if (errno == ENOENT && i == resolved_uri.size() - 1) {
				std::perror("open: NOT_FOUND");
				throw (HTTP_EXCEPTION(NOT_FOUND));
			}
			if (errno == EACCES) {
				std::perror("open: FORBIDDEN");
				throw (HTTP_EXCEPTION(FORBIDDEN));
			}
		}
	}
	return fd;
}

