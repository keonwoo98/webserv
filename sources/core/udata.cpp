//
// Created by 김민준 on 2022/12/21.
//

#include "udata.h"

Udata::Udata(int type, int fd) : type_(type), fd_(fd) {}

Udata::Udata(int type, Socket *socket) : type_(type), socket_(socket) {}


