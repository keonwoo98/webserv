#include "http_exception.hpp"
#include "status_code.hpp"

HttpException::HttpException(int status_code) : status_code_(status_code) {}

const char *HttpException::what() const throw() {
	if (status_code_ == BAD_REQUEST)
		return ("400: Bad Request");
	else if (status_code_ == FORBIDDEN)
		return ("403 : Forbidden");
	else if (status_code_ == NOT_FOUND)
		return ("404 : Not Found");
	else if (status_code_ == METHOD_NOT_ALLOWED)
		return ("405 : Method not allowed");
	else if (status_code_ == NOT_ACCEPTABLE)
		return ("406 : Not acceptable");
	else if (status_code_ == LENGTH_REQUIRED)
		return ("411 : Length Required");
	else if (status_code_ == PAYLOAD_TOO_LARGE)
		return ("413 : Payload too large");
	else if (status_code_ == URI_TOO_LONG)
		return ("414 : URI too long");
	else if (status_code_ == INTERNAL_SERVER_ERROR)
		return ("500 : Internal server error");
	else if (status_code_ == NOT_IMPLEMENTED)
		return ("501 : Not Implemented");
	else if (status_code_ == HTTP_VERSION_NOT_SUPPORTED)
		return ("505 : HTTP version not supported");
	else
		return ("Another Status");
}

int HttpException::GetStatusCode() const {
	return status_code_;
}
