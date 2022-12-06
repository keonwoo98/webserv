//
// Created by 김민준 on 2022/12/06.
//
#include "cgi_handler.hpp"
#include <iostream>
#include <unistd.h>

CgiHandler::CgiHandler() {
	ParseEnviron();
}

/**
 * PATH=/etc/nginx
 * key : PATH
 * value : /etc/nginx
 */
void CgiHandler::ParseEnviron() {
	int i = 0;
	while (environ[i++]) {
		std::string env_var(environ[i]);
		std::size_t found = env_var.find('=');
		if (found != std::string::npos) {
			std::string key = env_var.substr(0, found);
			std::string value = env_var.substr(found + 1, env_var.length());
			env.insert(std::make_pair(key, value));
		}
	}
}

void CgiHandler::AddEnv(const std::string &key, const std::string &value) {
	env.insert(std::make_pair(key, value));
}

void CgiHandler::SetCgiEnv() {
	AddEnv("QUERY_STRING", ""); // HTTP
	AddEnv("REQUEST_METHOD", "POST");
	AddEnv("CONTENT_TYPE", "application/x-www-form-urlencoded");
	AddEnv("CONTENT_LENGTH", "33");

	AddEnv("SCRIPT_FILENAME", "/Users/minjune/webserv/html/cgi-bin/gugu.php"); // SERVER
	AddEnv("SCRIPT_NAME", "/cgi-bin/gugu.php");

//	AddEnv("PATH_INFO", "");
//	AddEnv("PATH_TRANSLATED", "");

	AddEnv("REQUEST_URI", "/cgi-bin/gugu.php");
	AddEnv("DOCUMENT_URI", "/cgi-bin/gugu.php");
	AddEnv("SERVER_PROTOCOL", "HTTP/1.1");

	AddEnv("GATEWAY_INTERFACE", "CGI/1.1"); // SERVER INFO
	AddEnv("SERVER_SOFTWARE", "webserv/1.0");

	AddEnv("REMOTE_ADDR", "127.0.0.1"); // ADDRESS
	AddEnv("REMOTE_PORT", "62719");
	AddEnv("SERVER_ADDR", "127.0.0.1");
	AddEnv("SERVER_PORT", "8080");
	AddEnv("SERVER_NAME", "localhost");

//	AddEnv("HTTPS", "");

	AddEnv("REDIRECT_STATUS", "200");
}

char **CgiHandler::ConvertEnvToCharSequence() {
	char **envp = new char *[env.size()]; // new 실패시 예외 처리

	std::map<std::string, std::string>::const_iterator it = env.begin();
	size_t i = 0;
	while (i < env.size()) {
		std::string str = it->first + "=" + it->second;
		envp[i] = new char[str.length() + 1]; // new 실패시 예외 처리
		std::strcpy(envp[i], str.c_str());
		i++;
		it++;
	}
	return envp;
}

void CgiHandler::OpenPipe(int *pipe_fd) {
	if (pipe(pipe_fd) < 0) {
		std::perror("pipe: ");
	}
}

std::string CgiHandler::RunCgi() {
	char **env_list = ConvertEnvToCharSequence();

	int parent_pipe[2];
	int child_pipe[2];
	OpenPipe(parent_pipe);
	OpenPipe(child_pipe);

	pid_t pid = fork();
	if (pid < 0) {
		std::perror("fork: ");
		return NULL;
	}

	std::string method = env["REQUEST_METHOD"];

	if (pid == 0) { // CHILD
		// parent_pipe READ
		// child_pipe WRITE
		close(child_pipe[READ]);
		close(parent_pipe[WRITE]);

		dup2(parent_pipe[READ], STDIN_FILENO);
		dup2(child_pipe[WRITE], STDOUT_FILENO);

		char **argv = new char*[3];

		std::string php_cgi("/opt/homebrew/bin/php-cgi");
		argv[0] = new char[php_cgi.length() + 1];
		std::strcpy(argv[0], php_cgi.c_str());

		std::string php_file_path("/Users/minjune/webserv/html/cgi-bin/hello.php");
		argv[1] = new char[php_file_path.length() + 1];
		std::strcpy(argv[1], php_file_path.c_str());

		argv[2] = NULL;

		execve("/opt/homebrew/bin/php-cgi", argv, env_list);
		std::perror("execve : ");
	} else { // PARENT
		// child_pipe READ
		// parent_pipe WRITE
		close(child_pipe[WRITE]);
		close(parent_pipe[READ]);

		if (method == "POST") {
			std::string body = "input=2&answer=%EC%A0%9C%EC%B6%9C";
			char *body_c_str = new char[body.length() + 1];
			std::strcpy(body_c_str, body.c_str());

			ssize_t result = write(parent_pipe[WRITE], body_c_str, body.length() + 1);
			if (result < 0) {
				std::perror("write: ");
			}
		}

		wait(NULL);

		std::string cgi_result;
		while (true) {
			char buf[1024];
			ssize_t count = read(child_pipe[READ], buf, 1024);
			if (count < 0) {
				std::perror("read: ");
				break;
			}
			if (count == 0) {
				break;
			}
			cgi_result.append(buf, count);
		}
		std::cout << cgi_result << std::endl;

		// Status 200 302 400 501
		if (cgi_result.find("Status") != std::string::npos) {
			// document-response | client-redirect-response with document
			// Status 302 -> client-redirect-response
			// Status other -> document-response
		} else {
			// document-response (200 OK) | local-redirect-response | client-redirect-response
			// document-response -> Content-Type & body
			// local-redirect-response -> local-Location header (URI path and query-string)
			// client-redirect-response -> client-Location header (absolute URI path)
		}

		close(child_pipe[READ]);
		close(parent_pipe[WRITE]);
		return cgi_result;
	}
	return NULL;
}