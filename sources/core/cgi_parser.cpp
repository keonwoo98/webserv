#include "cgi_parser.hpp"
#include "character_const.hpp"

void ParseCgiResult(ResponseMessage &response) {
	const std::string &body = response.GetBody();
	size_t crlf = body.find(CRLF);
	if (crlf == std::string::npos) {
		throw HttpException(INTERNAL_SERVER_ERROR, "Parse Cgi Result find()");
	}
	while (crlf != 0) {
		std::string header_buf = body.substr(0, crlf);
		response.ParseHeader(header_buf);
		response.EraseBody(0, crlf + 2);
		crlf = body.find(CRLF);
	}
	response.EraseBody(0, 2); // CRLF
}
