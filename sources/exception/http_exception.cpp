#include "http_exception.hpp"
#include "status_code.hpp"

/**
 * Http Exception
 * @param status_code response message status code
 * @param message logs error message (ex) open error
 */
HttpException::HttpException(int status_code, const std::string &message)
	: err_msg_(message + "\n"), status_code_(status_code) {
	switch (status_code) {
		case OK :
			reason_phrase_ = "OK"; break;
		case BAD_REQUEST :
			reason_phrase_ = "Bad Request"; break;
		case FORBIDDEN :
			reason_phrase_ = "Forbidden"; break;
		case NOT_FOUND :
			reason_phrase_ = "Not Found"; break;
		case METHOD_NOT_ALLOWED :
			reason_phrase_ = "Method not allowed"; break;
		case NOT_ACCEPTABLE :
			reason_phrase_ = "Not acceptable"; break;
		case LENGTH_REQUIRED :
			reason_phrase_ = "Length Required"; break;
		case PAYLOAD_TOO_LARGE :
			reason_phrase_ = "Payload too large"; break;
		case URI_TOO_LONG :
			reason_phrase_ = "URI too long"; break;
		case INTERNAL_SERVER_ERROR :
			reason_phrase_ = "Internal server error"; break;
		case NOT_IMPLEMENTED :
			reason_phrase_ = "Not Implemented"; break;
		case HTTP_VERSION_NOT_SUPPORTED :
			reason_phrase_ = "HTTP version not supported"; break;
		default :
			reason_phrase_ = "Unknown error"; break;
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
