#include "cgi_parser.hpp"

void AppendCgiHeader(ResponseMessage &response, const char *buf,
					 const int size) {
	response.AppendBody(buf, size);
	const std::string &body = response.GetBody();
	size_t crlf = body.find("\r\n");
	if (crlf == std::string::npos) {
		return;
	}
	if (crlf == 0) {
		response.SetState(ResponseMessage::BODY);
		response.EraseBody(0, 2);
		return;
	}

	std::string header_buf = body.substr(0, crlf);
	size_t colon = header_buf.find(":");
	if (colon == std::string::npos) {
		throw CoreException::CgiExecutionException();
	}
    response.AddHeader(
		header_buf.substr(0, colon),
		header_buf.substr(colon + 2, header_buf.size() - (colon + 2)));
	response.EraseBody(0, crlf + 2);
}

void ParseCgiResult(ResponseMessage &response, const char *buf, const int size) {
    if (response.GetState() == ResponseMessage::HEADER) {
		AppendCgiHeader(response, buf, size);
	}
	else {
		response.AppendBody(buf, size);
	}
}
