//
// Created by 김민준 on 2022/12/21.
//

#ifndef WEBSERV_SOURCES_CORE_UDATA_H_
#define WEBSERV_SOURCES_CORE_UDATA_H_

#include "request_message.hpp"
#include "response_message.hpp"
#include "socket.hpp"

class Udata {
   public:
    enum State {
        LISTEN,
        RECV_REQUEST,
        READ_FILE,
        WRITE_TO_PIPE,
        READ_FROM_PIPE,
        SEND_RESPONE,
        CLOSE
    };
	explicit Udata(int type, int sock_d);
	virtual ~Udata();

	void ChangeType(int type);
	void Reset();

	int type_;
    int sock_d_;

	RequestMessage request_message_;
	ResponseMessage response_message_;
};

#endif	// WEBSERV_SOURCES_CORE_UDATA_H_

// OPEN_PIPE -> 이벤트 등록
// udata -> client -> response request
//
