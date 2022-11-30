#include "request_message.hpp"

RequestMessage::RequestMessage() {}
RequestMessage::~RequestMessage() {}

int RequestMessage::RecvMessage(int fd) {
	char buf[1024];
	int n = recv(fd, buf, sizeof(buf), 0);
	if (n <= 0) {
		if (n < 0) std::cerr << "client read error!" << std::endl;
		// disconnect_client(event_list[i].ident, clients);
	} else {
		buf[n] = '\0';
		header_ += buf;
	}
	return n;
}

void RequestMessage::ParsingMessage() {
	size_t pos = header_.find("\r\n\r\n");
	body_ = header_.substr(pos + 4, header_.size() - 1) + "\0";
	header_.erase(pos + 2, header_.size() - 1);
	header_ + "\0";
}
