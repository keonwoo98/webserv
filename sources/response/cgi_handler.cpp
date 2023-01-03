//
// Created by 김민준 on 2022/12/06.
//
#include "cgi_handler.hpp"

#include <unistd.h>

#include <iostream>
#include <cstdlib>

#include "client_socket.hpp"

CgiHandler::CgiHandler(const std::string &cgi_path) : cgi_path_(cgi_path) {}

CgiHandler::~CgiHandler() {}

/**
 * PATH=/etc/nginx
 * key : PATH
 * value : /etc/nginx
 */

void CgiHandler::ParseEnviron() {
	int i = 0;
	while (environ[i]) {
		std::string env_string(environ[i]);
		std::size_t found = env_string.find('=');
		if (found != std::string::npos) {
			std::string key = env_string.substr(0, found);
			std::string value =
				env_string.substr(found + 1, env_string.length() - (found + 1));
			cgi_envs_[key] = value;
		}
		++i;
	}
}

void CgiHandler::ConvertEnvToCharSequence() {
	env_list_ = new char *[cgi_envs_.size() + 1];  // new 실패시 예외 처리

	size_t i = 0;
	for (std::map<std::string, std::string>::const_iterator it =
			 cgi_envs_.begin();
		 it != cgi_envs_.end(); ++it, ++i) {
		std::string str = it->first + "=" + it->second;
		env_list_[i] = new char[str.length() + 1];
		std::strcpy(env_list_[i], str.c_str());
	}
	env_list_[i] = NULL;
}

std::string GetServerAddr(const int &fd) {
	return Socket::GetAddr(fd);
}

std::string GetServerPort(const int &fd) {
	return Socket::GetPort(fd);
}

void CgiHandler::SetCgiEnvs(const RequestMessage &request, ClientSocket *client_socket, const ServerInfo &server_info) {
	cgi_envs_["REQUEST_METHOD"] = request.GetMethod();	// METHOD
	cgi_envs_["REQUEST_URI"] = request.GetResolvedUri();
	cgi_envs_["SCRIPT_FILENAME"] = cgi_envs_["REQUEST_URI"];
	cgi_envs_["SCRIPT_NAME"] = cgi_envs_["REQUEST_URI"];

	if (cgi_envs_["REQUEST_METHOD"] == "GET") {
		cgi_envs_["QUERY_STRING"] = request.GetQuery();
	} else if (cgi_envs_["REQUEST_METHOD"] == "POST") {
		cgi_envs_["CONTENT_TYPE"] = request.GetHeaderValue("content-type");
		cgi_envs_["CONTENT_LENGTH"] = request.GetHeaderValue("content-length");
	}

	cgi_envs_["SERVER_PROTOCOL"] = "HTTP/1.1";	// HTTP version
	cgi_envs_["SERVER_SOFTWARE"] = "webserv/1.0";

	cgi_envs_["GATEWAY_INTERFACE"] = "CGI/1.1";	 // CGI
	cgi_envs_["REMOTE_ADDR"] = client_socket->GetAddr();
	cgi_envs_["REMOTE_PORT"] = client_socket->GetPort();

	cgi_envs_["SERVER_ADDR"] =
		GetServerAddr(client_socket->GetSocketDescriptor());
	cgi_envs_["SERVER_PORT"] =
		GetServerPort(client_socket->GetSocketDescriptor());
	cgi_envs_["SERVER_NAME"] = cgi_envs_["SERVER_ADDR"];
	cgi_envs_["REDIRECT_STATUS"] = "200";
	cgi_envs_["UPLOAD_PATH"] = server_info.GetUploadPath();
}

void CgiHandler::OpenPipe(KqueueHandler &kq_handler, Udata *user_data) {
	if (cgi_envs_["REQUEST_METHOD"] == "POST") {
		if (pipe(req_body_pipe_) < 0) {
			perror("pipe: ");
		}
		fcntl(req_body_pipe_[WRITE], F_SETFL, O_NONBLOCK);
		user_data->ChangeState(Udata::WRITE_TO_PIPE);
		kq_handler.AddWriteEvent(req_body_pipe_[WRITE], user_data);
	} else {
		user_data->ChangeState(Udata::READ_FROM_PIPE);
	}
	if (pipe(cgi_result_pipe_) < 0) {
		perror("pipe: ");
	}
	fcntl(cgi_result_pipe_[READ], F_SETFL, O_NONBLOCK);
	kq_handler.AddReadEvent(cgi_result_pipe_[READ], user_data);
}

void CgiHandler::SetupChildCgi() {
	if (cgi_envs_["REQUEST_METHOD"] == "POST") {
		close(req_body_pipe_[WRITE]);
		dup2(req_body_pipe_[READ], STDIN_FILENO);
	}
	close(cgi_result_pipe_[READ]);
	dup2(cgi_result_pipe_[WRITE], STDOUT_FILENO);
}

void CgiHandler::SetupParentCgi() {
	if (cgi_envs_["REQUEST_METHOD"] == "POST") {
		close(req_body_pipe_[READ]);
	}
	close(cgi_result_pipe_[WRITE]);
}

void CgiHandler::DetachChildCgi() {
	SetupChildCgi();

	char **argv = new char *[3];
	
	std::string php_cgi(cgi_path_);
	argv[0] = new char[php_cgi.length() + 1];
	std::strcpy(argv[0], php_cgi.c_str());

	std::string php_file_path(cgi_envs_["PATH_INFO"]);
	argv[1] = new char[php_file_path.length() + 1];
	std::strcpy(argv[1], php_file_path.c_str());

	argv[2] = NULL;

	execve(argv[0], argv, env_list_);
	std::perror("execve : ");
	exit(1);
}

void CgiHandler::SetupAndAddEvent(KqueueHandler &kq_handler, Udata *user_data,
								  ClientSocket *client_socket, const ServerInfo &server_info) {
	RequestMessage &request_message = user_data->request_message_;
	ParseEnviron();
	SetCgiEnvs(request_message, client_socket, server_info);
	ConvertEnvToCharSequence();
	OpenPipe(kq_handler, user_data);
	pid_t pid = fork();
	if (pid < 0) {
		std::perror("fork: ");
		return;
	}
	std::string method = cgi_envs_["REQUEST_METHOD"];
	if (pid == 0) {
		DetachChildCgi();
	} else {
		kq_handler.AddProcExitEvent(pid);
		SetupParentCgi();
	}
}
