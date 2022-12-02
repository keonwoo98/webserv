//
// Created by Alvin Lee on 2022/12/01.
//

#ifndef WEBSERV_GET_H
#define WEBSERV_GET_H

#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int GetMethod(std::string uri, std::string &body_entity);

#endif //WEBSERV_GET_H
