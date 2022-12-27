//
// Created by 김민준 on 2022/12/21.
//

#include "udata.hpp"

Udata::Udata(int state, int sock_d)
	: request_message_(-1), state_(state), sock_d_(sock_d) {}

Udata::~Udata() {}

const int &Udata::GetState() const { return state_; }

void Udata::ChangeState(const int &state) { state_ = state; }

void Udata::Reset() {
	state_ = RECV_REQUEST;
	response_message_.Clear();
	request_message_.Clear();
}
