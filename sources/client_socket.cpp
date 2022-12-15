#include "client_socket.hpp"

ClientSocket::ClientSocket(int sock_d)
	: parser_(request_),
	  response_(request_),
	  prev_state_(DONE),
	  state_(REQUEST) {
	type_ = Socket::CLIENT_TYPE;
	sock_d_ = sock_d;
}

ClientSocket::~ClientSocket() {}

const ClientSocket::State &ClientSocket::GetPrevState() const {
	return prev_state_;
}

void ClientSocket::SetPrevState(const ClientSocket::State &prev_state) {
	prev_state_ = prev_state;
}

const ClientSocket::State &ClientSocket::GetState() const { return state_; }

void ClientSocket::SetState(const ClientSocket::State &state) {
	state_ = state;
}

const int &ClientSocket::GetFileDescriptor() const { return file_d_; }

void ClientSocket::SetFileDescriptor(const int &file_d) { file_d_ = file_d; }

void ClientSocket::PrintRequest() const { std::cout << parser_; }

void ClientSocket::RecvRequest() {
	prev_state_ = state_;
	char tmp[1024];
	int n = recv(sock_d_, tmp, sizeof(tmp), 0);
	if (n <= 0) {
		std::cerr << "read error" << std::endl;
	} else {
		tmp[n] = '\0';
		parser_.AppendMessage(tmp);
	}
	if (parser_.IsDone()) {
		prev_state_ = state_;
		state_ = RESPONSE;
	}
}

void ClientSocket::ResetParsingState() { parser_.ResetState(); }

void ClientSocket::SendResponse() {
	response_.CreateMessage();
	buffer_ = response_.GetMessage();
	send(sock_d_, buffer_.c_str(), buffer_.length(), 0);
	buffer_.clear();
	prev_state_ = state_;
	state_ = DONE;
}
