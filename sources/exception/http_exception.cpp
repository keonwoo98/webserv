#include "http_exception.hpp"
#include "status_code.hpp"

/**
 * Http Exception
 * @param status_code response message status code
 * @param message log error message (ex) open error
 */
HttpException::HttpException(int status_code, const std::string &message)
	: err_msg_(message + "\n"), status_code_(status_code) {
	switch (status_code_) {
		case OK: reason_phrase_ = "OK";
			break;
		case NOT_FOUND: reason_phrase_ = "NOT FOUND";
			break;
		case INTERNAL_SERVER_ERROR: reason_phrase_ = "INTERNAL SERVER ERROR";
			break;
		case FORBIDDEN: reason_phrase_ = "FORBIDDEN";
			break;
		default: reason_phrase_ = "UNKNOWN";
			break;
	}
}

HttpException::~HttpException() throw() {}

const char *HttpException::what() const throw() {
	return (err_msg_.c_str());
}

int HttpException::GetStatusCode() const {
	return status_code_;
}

const std::string &HttpException::GetReasonPhrase() const {
	return reason_phrase_;
}
