//
// Created by Alvin Lee on 2022/12/01.
//
#include "get.hpp"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
extern int errno;
#define BUFFER_SIZE 1024

int GetMethod(std::string uri, std::string &body_entity) {
    char buf[BUFFER_SIZE];
    int fd = open(uri.c_str(), O_RDONLY);
    if (errno == 0) {
        int n = read(fd, buf, BUFFER_SIZE);
        std::cout << "read\n";
        close(fd);
        if (n < 0) {
            return 500;
        }
        body_entity = std::string(buf);
        return (200);
    }
    else if (errno == ENOENT) {
        std::cout << "ENOENT\n";
        close(fd);
        return 404;
    }
    else if (errno == EACCES) {
        std::cout << "EACCES\n";
        close(fd);
        return 403;
    }
    close(fd);
    return (0);
}
