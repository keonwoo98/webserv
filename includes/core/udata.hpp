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
		WRITE_TO_PIPE,
		READ_FROM_PIPE,
		SEND_RESPONSE,
		CLOSE
	};
	explicit Udata(int state, int sock_d);
	virtual ~Udata();

	const int &GetState() const;
	void ChangeState(const int &state);
	void Reset();

	RequestMessage request_message_;
	ResponseMessage response_message_;

	int state_;
	int sock_d_;
};

#endif	// WEBSERV_SOURCES_CORE_UDATA_H_

// OPEN_PIPE -> 이벤트 등록
// udata -> client -> response request
//
