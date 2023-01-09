//
// Created by 김민준 on 2022/12/21.
//

#ifndef WEBSERV_SOURCES_CORE_UDATA_HPP
#define WEBSERV_SOURCES_CORE_UDATA_HPP

#include "request_message.hpp"
#include "response_message.hpp"
#include "socket.hpp"

class Udata {
   public:
	enum State {
		LISTEN,
		RECV_REQUEST,
		READ_FILE,
		WRITE_FILE,
		CGI_PIPE,
		SEND_RESPONSE,
		CLOSE
	};
	Udata(int state, int sock_d);
	virtual ~Udata();

	int GetState() const;
	void ChangeState(int state);
	void Reset();

	RequestMessage request_message_;
	ResponseMessage response_message_;

	int state_;
	int sock_d_;
};

#endif	// WEBSERV_SOURCES_CORE_UDATA_HPP_
