#include "response_message.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

ResponseMessage::ResponseMessage(RequestMessage &request) : request_(request) {}

ResponseMessage::~ResponseMessage() {}

const std::string &ResponseMessage::GetStartLine() const { return start_line_; }

const std::string &ResponseMessage::GetHeaders() const { return headers_; }

const std::string &ResponseMessage::GetBody() const { return body_; }

const std::string ResponseMessage::GetMessage() {
	return start_line_ + headers_ + body_;
}

std::string ResponseMessage::GetStatus() const {
	std::string status;

	status = "200 OK";
	return status;
}

std::string ResponseMessage::GetDate() const {
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];
	int n;

	time(&rawtime);
	timeinfo = gmtime(&rawtime);
	n = strftime(buffer, 80, "%a, %d %b %G %T GMT", timeinfo);
	buffer[n] = '\0';
	return buffer;
}

std::string ResponseMessage::GetContentType() const { return "text/html"; }

std::string ResponseMessage::GetContentLength() const {
	return std::to_string(body_.length());
}

void ResponseMessage::CreateHeader() {
	headers_ = "HTTP/1.1 " + GetStatus() + "\r\n";
	headers_ += "Server: webserv\r\n";
	headers_ += "Date: " + GetDate() + "\r\n";
	headers_ += "Content-Type: " + GetContentType() + "\r\n";
	headers_ += "Content-Length: " + GetContentLength() + "\r\n";
	headers_ += "Last-Modified: " + GetDate() + "\r\n";
	headers_ += "Connection: keep-alive\r\n";
	headers_ += "ETag: 62d6ba2e-267\r\n";
	headers_ += "Accept-Ranges: bytes\r\n";
	headers_ += "\r\n";
}

void ResponseMessage::AppendBody(const std::string &message) {
	body_ += message;
}

void ResponseMessage::CreateBody() {
	Uri uri(request_.GetUri());
	std::ifstream open_file(uri.GetPath().data());
	std::cout << "\033[0mURI: " << request_.GetUri() << std::endl;
	std::cout << "\033[0mPATH: " << uri.GetPath() << std::endl;
	body_.clear();
	if (open_file.is_open()) {
		std::string line;
		while (getline(open_file, line)) {
			body_ += line + "\n";
		}
		open_file.close();
	}
}

void ResponseMessage::CreateMessage() {
	// CreateBody();
	CreateHeader();
}

void ResponseMessage::Clear() {
	headers_.clear();
	body_.clear();
}
