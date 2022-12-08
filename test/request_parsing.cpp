#include <fstream>
#include <iostream>

#include "../includes/request_message.hpp"

std::string sample_requet =
	"GET / HTTP/1.1\r\n"
	"Host: localhost:8181\r\n"
	"Connection: keep-alive\r\n"
	"Cache-Control: max-age=0\r\n"
	"Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\r\n"
	"Sec-Fetch-Site: same-origin\r\n"
	"Sec-Fetch-Mode: no-cors\r\n"
	"Sec-Fetch-Dest: image\r\n"
	"Referer: http://localhost:8181/\r\n"
	"Accept-Encoding: gzip, deflate, br\r\n"
	"Accept-Language: ko-KR,ko;q=0.9\r\n"
	"If-None-Match: 62d6ba2e-267\r\n"
	"If-Modified-Since: Wed, 30 Nov 2022 16:12:20 GMT\r\n"
	"\r\n";

std::string sample_chunked_request =
	"POST /upload HTTP/1.1\r\n"
	"User-Agent: Arduino\r\n"
	"Accept: */*\r\n"
	"Transfer-Encoding: chunked\r\n"
	"\r\n"
	"26 ; some-extension\r\n" //38
	"this is the text, of this file, wooo! \r\n"
	"1d\r\n" //29
	"more test, of this file,luck\n\r\n"
	"0 ; some-ext_name=some-ext_value\r\n"
	"SomeTrailerName: SomTrailerValue\r\n"
	"SomeTrailerName2: SomTrailerValue2\r\n"
	"\r\n";

int main() {
	// std::string filePath = "ex/request-msg";
	// std::string buf;

	// // read File
	// std::ifstream openFile(filePath.data());
	// if (openFile.is_open()) {
	// 	std::string line;
	// 	while (getline(openFile, line)) {
	// 		buf += line + "\n";
	// 	}
	// 	openFile.close();
	// }

	RequestMessage request;
	request.ParsingMessage(sample_chunked_request);
	std::cout << "< start line >" << std::endl;
	std::cout << request.GetMethod()<< std::endl;
	std::cout << request.GetUri() << std::endl;
	
	std::cout << "< header field >" << std::endl;
	RequestMessage::header_map_type header_map = request.GetHeaderMap();
	for (std::map<std::string, std::string>::iterator it =
			 header_map.begin();
		 it != header_map.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}

	std::cout << "< body >" << std::endl;
	std::cout << request.GetBody() << std::endl;

	return 0;
}
