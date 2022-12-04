#include <fstream>
#include <iostream>

#include "../includes/request_message.hpp"

int main() {
	std::string filePath = "ex/request-msg";
	std::string buf;

	// read File
	std::ifstream openFile(filePath.data());
	if (openFile.is_open()) {
		std::string line;
		while (getline(openFile, line)) {
			buf += line + "\n";
		}
		openFile.close();
	}

	RequestMessage request;
	request.ParsingMessage(buf);
	std::cout << request.method_ << std::endl;
	std::cout << request.uri_ << std::endl;
	for (std::map<std::string, std::string>::iterator it =
			 request.header_map_.begin();
		 it != request.header_map_.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}

	return 0;
}
