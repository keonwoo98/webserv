#include "response_message.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

ResponseMessage::ResponseMessage() {}
ResponseMessage::~ResponseMessage() {}

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

std::string ResponseMessage::GetContentType() const {
	return "text/html";
}

std::string ResponseMessage::GetContentLength() const {
	return std::to_string(body_.length());
}

void ResponseMessage::SetHeader() {
	header_ = "HTTP/1.1 " + GetStatus() + "\r\n";
	header_ += "Server: webserv\r\n";
	header_ += "Date: " + GetDate() + "\r\n";
	header_ += "Content-Type: " + GetContentType() + "\r\n";
	header_ += "Content-Length: " + GetContentLength() + "\r\n";
	header_ += "Last-Modified: " + GetDate() + "\r\n";
	header_ += "Connection: keep-alive\r\n";
	header_ += "ETag: 62d6ba2e-267\r\n";
	header_ += "Accept-Ranges: bytes\r\n";
	header_ += "\r\n";
}

void ResponseMessage::SetBody() {
	std::string file_path = "index.html";
	std::ifstream open_file(file_path.data());
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
	SetBody();
	SetHeader();
}

std::string ResponseMessage::GetMessage() {
	return header_ + body_;
}
