#include <unistd.h>

#include <iostream>
#include <vector>

extern char **environ;

char **makeEnvList(std::vector<std::string> cgi_envs) {
	char **ret = new char *[cgi_envs.size() + 1];

	int i = 0;
	for (std::vector<std::string>::iterator it = cgi_envs.begin();
		 it != cgi_envs.end(); ++it, ++i) {
		std::string str = *it;
		ret[i] = new char[str.size()];
		strcpy(ret[i], str.c_str());
	}
	ret[i] = NULL;
	return ret;
}

char **makeCgiEnvs() {
	std::vector<std::string> cgi_envs;
	int i = 0;
	while (environ[i]) {
		cgi_envs.push_back(std::string(environ[i]));
		++i;
	}
	cgi_envs.push_back(std::string("REQUEST_METHOD=POST"));
	cgi_envs.push_back(std::string("REQUEST_URI=./docs/cgi-bin/upload.php"));
	cgi_envs.push_back(std::string("SCRIPT_FILENAME=./docs/cgi-bin/upload.php"));
	cgi_envs.push_back(std::string("SCRIPT_NAME=./docs/cgi-bin/upload.php"));
	cgi_envs.push_back(std::string("CONTENT_TYPE=multipart/form-data; boundary=------WebKitFormBoundary0RCiTLuzHePIvvGu"));
	cgi_envs.push_back(std::string("CONTENT_LENGTH=666"));
	cgi_envs.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
	cgi_envs.push_back(std::string("SERVER_SOFTWARE=webserv/1.0"));

	cgi_envs.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
	cgi_envs.push_back(std::string("REMOTE_ADDR=127.0.0.1"));
	cgi_envs.push_back(std::string("REMOTE_PORT=1277"));

	cgi_envs.push_back(std::string("SERVER_ADDR=127.0.0.1"));
	cgi_envs.push_back(std::string("SERVER_PORT=8181"));
	cgi_envs.push_back(std::string("SERVER_NAME=localhost"));
	cgi_envs.push_back(std::string("REDIRECT_STATUS=200"));
	return makeEnvList(cgi_envs);
}

int main() {
	

	char **argv = new char *[3];

	std::string php_cgi("/opt/homebrew/bin/php-cgi");
	argv[0] = new char[php_cgi.length() + 1];
	std::strcpy(argv[0], php_cgi.c_str());

	std::string php_file_path("./docs/cgi-bin/post_result.php");
	argv[1] = new char[php_file_path.length() + 1];
	std::strcpy(argv[1], php_file_path.c_str());

	argv[2] = NULL;

	execve(argv[0], argv, makeCgiEnvs());
	std::perror("execve : ");
}
