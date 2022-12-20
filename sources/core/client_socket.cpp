#include "client_socket.hpp"

#include <arpa/inet.h>

const int ClientSocket::BUFFER_SIZE = 1024;

ClientSocket::ClientSocket(int sock_d)
	: parser_(request_),
	  response_(request_),
	  prev_state_(INIT),
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

bool ClientSocket::IsStateChanged() const { return prev_state_ != state_; }

void ClientSocket::PrintRequest() const { std::cout << parser_; }

void ClientSocket::RecvRequest() {
	prev_state_ = state_;
	char tmp[BUFFER_SIZE];
	int n = recv(sock_d_, tmp, sizeof(tmp), 0);
	if (n <= 0) {
		std::cerr << "recv error" << std::endl;
	} else {
		tmp[n] = '\0';
		// std::cout << tmp << std::endl; // for debugging
		parser_.AppendMessage(tmp);
	}
	if (parser_.IsDone()) {
		OpenFile(O_RDONLY);
		ChangeState(READ_FILE);
	}
}

void ClientSocket::OpenFile(int mode) {
	Uri uri(request_.GetUri());
	file_d_ = open(uri.GetPath().c_str(), mode);
	std::cout << uri.GetPath() << std::endl;
	if (file_d_ < 0) {
		perror("open error");
	}
	if (fcntl(file_d_, F_SETFL, O_NONBLOCK) < 0) {
		perror("fcntl error");
	}
}

void ClientSocket::ReadFile(intptr_t data) {
	char tmp[BUFFER_SIZE];
	prev_state_ = state_;
	int n = read(file_d_, tmp, sizeof(tmp));
	if (n < 0) {
		std::cerr << "read error" << std::endl;
		// throw HTTP_EXCEPTION(400);
	} else {
		tmp[n] = '\0';
		response_.AppendBody(tmp);
	}
	if (n < 0 || data <= n) {
		close(file_d_);
		ChangeState(RESPONSE);
	}
}

void ClientSocket::SendResponse() {
	PrintRequest(); // for debugging
	prev_state_ = state_;
	response_.CreateMessage();
	buffer_ = response_.GetMessage();
	send(sock_d_, buffer_.c_str(), buffer_.length(), 0);
	ChangeState(REQUEST);
	ResetSocket();
}

void ClientSocket::ChangeState(ClientSocket::State state) {
	prev_state_ = state_;
	state_ = state;
}

void ClientSocket::ResetSocket() {
	buffer_.clear();
	parser_.Reset();
	request_.Clear();
	response_.Clear();
}
