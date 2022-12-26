//
// Created by Alvin Lee on 2022/12/01.
//
#include "get_method.hpp"
#include "status_code.hpp"

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int GetMethod(std::string uri, std::string &body_entity) {
	char buf[BUFFER_SIZE + 1];
	int fd = open(uri.c_str(), O_RDONLY);
	if (fd < 0) {
		if (errno == ENOENT) {
			std::perror("open: NOT_FOUND");
			return NOT_FOUND;
		}
		if (errno == EACCES) {
			std::perror("open: FORBIDDEN");
			return FORBIDDEN;
		}
	}
	int size = read(fd, buf, BUFFER_SIZE);
	while (size > 0) { // NONBLOCK 처리 필요
		buf[size] = 0;
		body_entity.append(buf);
		size = read(fd, buf, BUFFER_SIZE);
	}
	close(fd);
	if (size < 0) {
		body_entity.clear();
		std::perror("open: INTERNAL_SERVER_ERROR");
		return INTERNAL_SERVER_ERROR;
	}
	return OK;
}
