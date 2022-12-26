//
// Created by Alvin Lee on 2022/12/02.
//

//#include "request_message.hpp" // 나중에 merge할때
#include "post_method.hpp"
#include "status_code.hpp"

#include <sys/fcntl.h>
#include <unistd.h>
#include <errno.h>
#define BUFFER_SIZE 1024


static bool IsCGI(std::string uri, std::string cgi_uri) {
	// conf file 의 location block 안에 cgi_path 가 있는지 없는지 판단
	// TEST
	if (uri.find(cgi_uri) == 0)
		return true;
	return false;
}
//location /abcgi/ {
//    cgi_path: phphcgi;
//
//};

int PostMethod(std::string uri, std::string &body_entity) {
	std::string cgi_uri("/cgi/"); // TEST 일단 cgi_path를 '/cgi' 로 초기화 함
	std::string form_uri("/"); // form data로 요청이 들어왔을때.
	std::string root("./");

	// if cgi
	if (IsCGI(uri, cgi_uri)) {
		// run CGI
		return OK;
	}
	else {
		char buf[BUFFER_SIZE + 1];
		int fd = open((root + uri).c_str(), O_RDONLY);
		if (fd < 0) {
			if (errno == ENOENT) {
				std::perror("open: NOT_FOUND");
				return NOT_FOUND;
			}
			if (errno == EACCES) {
				std::perror("open: FORBIDDEN");
				return FORBIDDEN;
			}
		}
		int size = read(fd, buf, BUFFER_SIZE);
		while (size > 0) { // NONBLOCK 처리 필요
			buf[size] = 0;
			body_entity.append(buf);
			size = read(fd, buf, BUFFER_SIZE);
		}
		close(fd);
		if (size < 0) {
			body_entity.clear();
			std::perror("open: INTERNAL_SERVER_ERROR");
			return INTERNAL_SERVER_ERROR;
		}
	}
	return OK;
}
