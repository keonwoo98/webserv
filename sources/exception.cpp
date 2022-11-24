#include <exception>

class SocketCreationException : public std::exception {
   public:
	const char *what() const throw() { return "socket() error"; }
};

class SetSocketExecption : public std::exception {
   public:
	const char *what() const throw() { return "setsockopt() error"; }
};

class BindException : public std::exception {
   public:
	const char *what() const throw() { return "bind() error"; }
};

class ListenException : public std::exception {
   public:
	const char *what() const throw() { return "listen() error"; }
};

class AcceptException : public std::exception {
   public:
	const char *what() const throw() { return "accpet() error"; }
};

class KqueueException : public std::exception {
   public:
	const char *what() const throw() { return "kqueue() error"; }
};

class RecvException : public std::exception {
	public:
	 const char *what() const throw() { return "recv() error"; }
};

class SendException : public std::exception {
	public:
	 const char *what() const throw() { return "send() error"; }
};
