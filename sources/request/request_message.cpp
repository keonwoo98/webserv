#include "request_message.hpp"

#include <fstream>
#include <locale>
#include <sstream>

RequestMessage::RequestMessage(int client_max_body_size)
  : client_max_body_size_(client_max_body_size) {
	this->Clear();
}

RequestMessage::~RequestMessage() {}

void RequestMessage::Clear() {
	status_code_ = CONTINUE;
	content_size_ = -1;
	is_chunked_ = false;
	keep_alive_ = true;

	state_ = START_METHOD;
	temp_header_name_.clear();
	temp_header_value_.clear();

	last_chunk_flag_ = false;
	chunk_size_ = 0;
	chunk_size_str_.clear();
	chunk_body_.clear();
	error_msg_.clear();

	method_.clear();
	uri_.clear();
	http_version_.clear();
	headers_.clear();
	body_.clear();
}

bool RequestMessage::IsThereHost() const {
	if (headers_.find("host") != headers_.end()) {
		return true;
	}
	return false;
}

bool RequestMessage::CheckHeaderName() const {
	if (temp_header_name_.size() == 0)
		return false;
	if (headers_.find(temp_header_name_) == headers_.end())
		return true;
	return false;
}

void RequestMessage::AddHeaderField() {
	headers_.insert(std::make_pair(temp_header_name_, temp_header_value_));
	const std::string &key = temp_header_name_;
	const std::string &value = temp_header_value_;

	if (key == "connection") {
		if (value.find("close")) {
			keep_alive_ = false;
		}
	} else if (key == "content-length") {
		content_size_ = atoi(value.c_str());
	} else if (key == "transfer-encoding" && method_ == "POST") {
		is_chunked_ = true;
	}

	temp_header_name_.clear();
	temp_header_value_.clear();
}



std::ostream &operator<<(std::ostream &os, const RequestMessage &req_msg) {
	os << C_ITALIC << C_LIGHTCYAN << "======[    Request Message     ]========" << C_RESET << C_FAINT << C_CYAN << std::endl;
	os << C_UNDERLINE << "Method" << C_RESET << C_FAINT << C_CYAN <<  ": " << req_msg.GetMethod() << std::endl;
	os << C_UNDERLINE << "Target" << C_RESET << C_FAINT << C_CYAN <<  ": " << req_msg.GetUri() << std::endl;
	os << C_UNDERLINE << "Heades" << C_RESET << C_FAINT << C_CYAN <<  ": " << std::endl;
	RequestMessage::headers_type::const_iterator it;
	for (it = req_msg.GetHeaders().begin(); it !=  req_msg.GetHeaders().end() ; it++)
		os << "  " << it->first << ": " << it->second << std::endl;
	os << C_UNDERLINE << "Body" << C_RESET << C_FAINT << C_CYAN <<  ": " << std::endl;
	os << req_msg.GetBody() << C_RESET << std::endl;
	os << C_ITALIC << C_LIGHTCYAN << "---------------------------------------" << std::endl;
	os << "[StatusCode] : " << req_msg.GetStatusCode() << std::endl;
	os << "[Connection] : " << (req_msg.IsAlive() ? "alive" : "close") << std::endl;
	os << "=======================================" << C_RESET << std::endl;
	return os;
}
