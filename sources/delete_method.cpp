//
// Created by 김민준 on 2022/12/01.
//

/**
 * DELETE Method
 *
 * Directory 삭제 시도 -> 409 Conflict
 * 없는 파일 삭제 시도 -> 404 Not Found
 */
#include "delete_method.hpp"
#include "status_code.hpp"

#include <string>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>


int DeleteMethod(const std::string &uri) {
	int fd = open(uri.c_str(), O_RDWR);
	if (fd < 0) {
		std::perror("open: ");
		if (errno == ENOENT) {
			return NOT_FOUND;
		}
		return CONFLICT;
	}
	if (unlink(uri.c_str()) < 0) {
		std::perror("unlink: ");
		return CONFLICT;
	}
	return OK;
}