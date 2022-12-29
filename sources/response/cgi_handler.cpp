//
// Created by 김민준 on 2022/12/06.
//
#include "cgi_handler.hpp"

#include <unistd.h>

#include <iostream>

#include "client_socket.hpp"

CgiHandler::CgiHandler() {
}

CgiHandler::~CgiHandler() {}

/**
 * PATH=/etc/nginx
 * key : PATH
 * value : /etc/nginx
 */

void CgiHandler::ParseEnviron() {
	int i = 0;
	while (environ[i]) {
		std::string env_var(environ[i]);
		std::size_t found = env_var.find('=');
		if (found != std::string::npos) {
			std::string key = env_var.substr(0, found);
			std::string value = env_var.substr(found + 1, env_var.length());
			cgi_envs_[key] = value;
		}
		++i;
	}
}

void CgiHandler::ConvertEnvToCharSequence() {
	env_list_ = new char *[cgi_envs_.size()];  // new 실패시 예외 처리

	std::map<std::string, std::string>::const_iterator it = cgi_envs_.begin();
	size_t i = 0;
	while (i < cgi_envs_.size()) {
		std::string str = it->first + "=" + it->second;
		env_list_[i] = new char[str.length() + 1];	// new 실패시 예외 처리
		std::strcpy(env_list_[i], str.c_str());
		++i;
		++it;
	}
	env_list_[i] = NULL;
}

std::string GetAddrFromSocketDescriptor(const int &fd) {
	(void)fd;
	// struct sockaddr_in addr = {};
	// socklen_t addr_len = sizeof(addr);

	// getsockname(fd, (struct sockaddr *) &addr, &addr_len);
	// return Socket::HostToIpAddr(addr.sin_addr);
	return "127.0.0.1";
}

std::string GetPortFromSocketDescriptor(const int &fd) {
	(void)fd;
	// struct sockaddr_in addr = {};
	// socklen_t addr_len = sizeof(addr);

	// return ntohs(addr.sin_port);
	return "8181";
}

void CgiHandler::SetCgiEnvs(const RequestMessage &request, ClientSocket *client_socket) {
	cgi_envs_["REQUEST_METHOD"] = request.GetMethod();	// METHOD
	// "SCRIPT_FILENAME", "/Users/minjune/webserv/html/cgi-bin/gugu.php"
	// cgi_envs_["REQUEST_URI"] = request.GetResolvedUri();
	cgi_envs_["REQUEST_URI"] = "/Users/zhy2on/Documents/www/get.php";
	cgi_envs_["PATH_INFO"] = cgi_envs_["REQUEST_URI"];
	cgi_envs_["DOCUMENT_URI"] = cgi_envs_["REQUEST_URI"];
	cgi_envs_["SCRIPT_NAME"] = cgi_envs_["REQUEST_URI"];
	if (request.GetMethod() == "GET") {
		// cgi_envs_["QUERY_STRING"] = request.GetQuery();
		cgi_envs_["QUERY_STRING"] = "id=a&age=b";
	}

	cgi_envs_["SERVER_PROTOCOL"] = "HTTP/1.1";	// HTTP version
	cgi_envs_["SERVER_SOFTWARE"] = "webserv/1.0";
	if (request.GetMethod() == "POST") {
		cgi_envs_["CONTENT_TYPE"] = request.GetHeaderValue("content-type");
		cgi_envs_["CONTENT_LENGTH"] = request.GetHeaderValue("content-length");
	}

	cgi_envs_["GATEWAY_INTERFACE"] = "CGI/1.1";	 // CGI
	cgi_envs_["REMOTE_ADDR"] = client_socket->GetAddr();
	cgi_envs_["REMOTE_PORT"] = client_socket->GetPort();

	cgi_envs_["SERVER_ADDR"] =
		GetAddrFromSocketDescriptor(client_socket->GetSocketDescriptor());
	cgi_envs_["SERVER_PORT"] =
		GetPortFromSocketDescriptor(client_socket->GetSocketDescriptor());
	cgi_envs_["SERVER_NAME"] = cgi_envs_["SERVER_ADDR"];

	// "REDIRECT_STATUS", "200"
}

void CgiHandler::OpenPipe(KqueueHandler &kq_handler, Udata *user_data) {
	if (cgi_envs_["REQUEST_METHOD"] == "POST") {
		if (pipe(req_body_pipe_) < 0) {
			perror("pipe: ");
		}
		user_data->ChangeState(Udata::WRITE_TO_PIPE);
		kq_handler.AddWriteEvent(req_body_pipe_[WRITE], user_data);
	} else {
		user_data->ChangeState(Udata::READ_FROM_PIPE);
	}
	if (pipe(cgi_result_pipe_) < 0) {
		perror("pipe: ");
	}
	kq_handler.AddReadEvent(cgi_result_pipe_[READ], user_data);
}

void CgiHandler::SetupCgiResultPipe() {
	if (cgi_envs_["REQUEST_METHOD"] == "POST") {
		close(req_body_pipe_[WRITE]);
		dup2(req_body_pipe_[READ], STDIN_FILENO);
	}
	close(cgi_result_pipe_[READ]);
	dup2(cgi_result_pipe_[WRITE], STDOUT_FILENO);
}

void CgiHandler::SetupReqBodyPipe() {
	if (cgi_envs_["REQUEST_METHOD"] == "POST") {
		close(req_body_pipe_[READ]);
	}
	close(cgi_result_pipe_[WRITE]);
}

const std::string &GetCgiExecutePath(ClientSocket *client_socket) {
	return client_socket->GetServerInfo()
		.GetLocations()
		.at(client_socket->GetLocationIndex())
		.GetCgi()
		.at(1);
}

void CgiHandler::RunChildCgi(const RequestMessage &request_message) {
	SetupCgiResultPipe();

	char **argv = new char *[3];
	
	(void)request_message;
	// std::string php_cgi(request_message.GetCgiPath());
	std::string php_cgi("/opt/homebrew/bin/php");
	argv[0] = new char[php_cgi.length() + 1];
	std::strcpy(argv[0], php_cgi.c_str());

	std::string php_file_path(cgi_envs_["PATH_INFO"]);
	argv[1] = new char[php_file_path.length() + 1];
	std::strcpy(argv[1], php_file_path.c_str());

	argv[2] = NULL;

	execve(argv[0], argv, env_list_);
	std::perror("execve : ");
	exit(0);
}

void CgiHandler::SetupAndAddEvent(KqueueHandler &kq_handler, Udata *user_data,
								  ClientSocket *client_socket) {
	RequestMessage &request_message = user_data->request_message_;
	ParseEnviron();
	SetCgiEnvs(request_message, client_socket);
	ConvertEnvToCharSequence();
	OpenPipe(kq_handler, user_data);

	pid_t pid = fork();
	if (pid < 0) {
		std::perror("fork: ");
		return;
	}
	std::string method = cgi_envs_["REQUEST_METHOD"];
	if (pid == 0) {
		RunChildCgi(request_message);
	} else {
		SetupReqBodyPipe();
		return;
	}
}
