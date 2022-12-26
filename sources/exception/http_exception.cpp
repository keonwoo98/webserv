#include "http_exception.hpp"

HttpException::HttpException(int status_code) : status_code_(status_code) {}

const char *HttpException::what() const throw() {
	if (status_code_ == 400) {
		return "400 Bad Request";
	} else if (status_code_ == 401) {
		return "401 Unauthorized";
	} else if (status_code_ == 501) {
		return "501 Not Implemented";
	}
	return "";
}

int HttpException::GetStatusCode() const {
    return status_code_;
}
