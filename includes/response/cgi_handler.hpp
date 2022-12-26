//
// Created by 김민준 on 2022/12/04.
//

#ifndef WEBSERV_INCLUDES_CGI_HANDLER_H_
#define WEBSERV_INCLUDES_CGI_HANDLER_H_

extern char **environ;

#include <map>
#include <string>

#define READ 0
#define WRITE 1

class CgiHandler {
   private:
	std::map<std::string, std::string> env;
	void ParseEnviron();
	char **ConvertEnvToCharSequence();
	void OpenPipe(int *pipe_fd);

   public:
	CgiHandler();
	void AddEnv(const std::string &key, const std::string &value);
	void SetCgiEnv();
	std::string RunCgi();
};

#endif	// WEBSERV_INCLUDES_CGI_HANDLER_H_
