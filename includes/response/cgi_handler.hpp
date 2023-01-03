//
// Created by 김민준 on 2022/12/04.
//

#ifndef WEBSERV_INCLUDES_CGI_HANDLER_H_
#define WEBSERV_INCLUDES_CGI_HANDLER_H_

extern char **environ;

#include <map>
#include <string>

#include "kqueue_handler.hpp"
#include "request_message.hpp"
#include "udata.hpp"

#define READ 0
#define WRITE 1

class ClientSocket;

class CgiHandler {
   public:
	CgiHandler(const std::string &cgi_path);
	~CgiHandler();

	void SetupAndAddEvent(KqueueHandler &kq_handler, Udata *user_data,
						  ClientSocket *client_socket, const ServerInfo &server_info);

	void OpenPipe(KqueueHandler &kq_handler, Udata *user_data);

	void SetCgiEnvs(const RequestMessage &request, ClientSocket *client_socket, const ServerInfo &server_info);

	void SetupChildCgi();
	void SetupParentCgi();

	void DetachChildCgi();

   private:
    const std::string cgi_path_;

	char **env_list_;
	std::map<std::string, std::string> cgi_envs_;

	int req_body_pipe_[2];
	int cgi_result_pipe_[2];

	void ParseEnviron();
	void ConvertEnvToCharSequence();
};

#endif	// WEBSERV_INCLUDES_CGI_HANDLER_H_
