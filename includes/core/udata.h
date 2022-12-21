//
// Created by 김민준 on 2022/12/21.
//

#ifndef WEBSERV_SOURCES_CORE_UDATA_H_
#define WEBSERV_SOURCES_CORE_UDATA_H_

#include "socket.hpp"

#define LISTEN 0
#define RECV_REQUEST 1
#define SEND_RESPONSE 2
#define READ_FILE 3
#define PIPE_READ 4
#define PIPE_WRITE 5

class Udata {
   public:
	Udata(int type, int fd);
	Udata(int type, Socket *socket);

	int type_;
	int fd_;
	Socket *socket_;
};

#endif //WEBSERV_SOURCES_CORE_UDATA_H_
