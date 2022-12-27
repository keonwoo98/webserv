#include "http_exception.hpp"
#include "status_code.hpp"

HttpException::HttpException(int status_code, const std::string &message)
: status_code_(status_code) {
	err_msg_ = std::to_string(status_code) + " | " + message;
}

HttpException::~HttpException() throw() {}

const char *HttpException::what() const throw() {
	return (err_msg_.c_str());
}

int HttpException::GetStatusCode() const {
	return status_code_;
}
