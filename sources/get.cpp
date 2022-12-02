//
// Created by Alvin Lee on 2022/12/01.
//
#include "get.hpp"
#include "status_code.hpp"

extern int errno;

int GetMethod(std::string uri, std::string &body_entity) {
    char buf;
    char rtn[OPEN_MAX];
    int i = 0;
    int fd = open(uri.c_str(), O_RDONLY);
    if (fd < 0) {
        if (errno == ENOENT) {
            std::perror("open: NOT_FOUND");
            return NOT_FOUND;
        }
        else if (errno == EACCES) {
            std::perror("open: FORBIDDEN");
            return FORBIDDEN;
        }
    }
    int size = read(fd, &buf, 1);
    while (size > 0) {
        rtn[i++] = buf;
        size = read(fd, &buf, 1);
    }
    rtn[i] = 0;
    close(fd);
    if (size < 0) {
        std::perror("open: INTERNAL_SERVER_ERROR");
        return INTERNAL_SERVER_ERROR;
    }
    body_entity.append(rtn);
    return OK;
}
