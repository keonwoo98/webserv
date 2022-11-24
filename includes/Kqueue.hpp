#ifndef KQUEUE_HPP
# define KQUEUE_HPP

# include "../includes/Socket.hpp"

class Kqueue
{
private :
	int		kq_;
	Socket	socket_;
	Kqueue(const Kqueue & Kq);
	Kqueue & operator=(const Kqueue & Kq);

	void setListeningEvents();
	void eventError();
	void eventEof(int sock_fd);
	void eventRead(int sock_fd);
	void eventWrite(int sock_fd);

	void setReadEvent(int sock_fd);
	void setWriteEvent(int sock_fd);
public :
	Kqueue();
	explicit Kqueue(const std::vector<std::string> & ports);
	~Kqueue() {}


	void eventLoop();
};

#endif
