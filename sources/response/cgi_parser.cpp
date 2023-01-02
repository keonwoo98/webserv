#include "cgi_parser.hpp"
#include "character_const.hpp"

void ParseCgiResult(ResponseMessage &response) {
	const std::string &body = response.GetBody();
	size_t crlf = body.find(CRLF);
	if (crlf == std::string::npos) {
		throw CoreException::CgiExecutionException();
	}

	while (crlf != 0) {
		std::string header_buf = body.substr(0, crlf);
		std::cout << header_buf << std::endl;
		size_t colon = header_buf.find(":");
		if (colon == std::string::npos) {
			throw CoreException::CgiExecutionException();
		}
		response.AddHeader(
			header_buf.substr(0, colon),
			header_buf.substr(colon + 2, header_buf.size() - (colon + 2)));
		response.EraseBody(0, crlf + 2);
		crlf = body.find(CRLF);
	}
	response.EraseBody(0, 2);
}
