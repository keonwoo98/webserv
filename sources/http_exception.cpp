#include "http_exception.hpp"

const char *HttpException::E400::what() const throw()
{
	return ("400 Bad Request");
}

const char *HttpException::E401::what() const throw()
{
	return ("401 Unauthorized");
}

const char *HttpException::E501::what() const throw()
{
	return ("501 Not Implemented");
}
