//
// Created by 김민준 on 2022/12/21.
//

#ifndef WEBSERV_SOURCES_CORE_UDATA_H_
#define WEBSERV_SOURCES_CORE_UDATA_H_

#include "socket.hpp"
#include "response_message.hpp"
#include "request_message.hpp"

#define LISTEN 0
#define RECV_REQUEST 1
#define SEND_RESPONSE 2
#define READ_FILE 3
#define PIPE_READ 4
#define PIPE_WRITE 5

class Udata {
   public:
	explicit Udata(int type);
	virtual ~Udata();

	void ChangeType(int type);
	void Reset();

	int type_;

	RequestMessage request_message_;
	ResponseMessage response_message_;
};

#endif //WEBSERV_SOURCES_CORE_UDATA_H_

// OPEN_PIPE -> 이벤트 등록
// udata -> client -> response request
//
