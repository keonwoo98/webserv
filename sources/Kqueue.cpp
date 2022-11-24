#include <iostream>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include "../includes/Kqueue.hpp"

void	Kqueue::setListeningEvents()
{

	Socket::listening_map_t::const_iterator b_it = socket_.getListeningConnections().begin();
	Socket::listening_map_t::const_iterator e_it = socket_.getListeningConnections().end();

	for (; b_it != e_it ; b_it++)
		setReadEvent(b_it->first);
}

Kqueue::Kqueue(const std::vector<std::string> & ports)
: kq_(kqueue()), socket_(ports)
{
	if (-1 == this->kq_)
		throw std::runtime_error("[Kqeue] : kqueue init error");
	std::cout << "kqueue created : #" << kq_ << std::endl;
	this->socket_.listen();
	this->setListeningEvents();
}

void Kqueue::eventLoop()
{
	int				num_of_events;
	struct kevent	return_event[10]; // <- 임의의 수. 나중에 정해야 할 듯.


	while (true) // loop
	{
		std::cout << "#"<< kq_ << " event loop : " << std::endl;
		num_of_events = kevent(kq_, NULL, 0, return_event, 10, 0);
		std::cout << "event return : " << num_of_events << std::endl;
		if (-1 == num_of_events)
			throw std::runtime_error("[Event Loop] : kevent() error");
		for (int i = 0 ; i < num_of_events ; i++)
		{
			int event_fd = return_event[i].ident;
			if (return_event[i].flags & EV_ERROR) // error
				eventError();
			else if (return_event[i].flags & EV_EOF) // disconnect
				eventEof(event_fd);
			else if (return_event[i].filter & EVFILT_READ) // accept or recv
				eventRead(event_fd);
			else if (return_event[i].filter & EVFILT_WRITE) // send
				eventWrite(event_fd);
			else
				throw std::runtime_error("[Event Loop] : unexpected case");
		}
	}
}

void Kqueue::eventError()
{
	throw std::runtime_error("[Event Loop] : Error");
}

void Kqueue::eventEof(int sock_fd)
{
	socket_.close(sock_fd);
	std::cout << "Client has disconnected" << std::endl;
}

void Kqueue::eventRead(int sock_fd)
{
	std::cout << "SOMETHING CAME : " << sock_fd << std::endl;
	if (socket_.isClientSocket(sock_fd) == true)
	{
		socket_.receive(sock_fd);
		//setWriteEvent(sock_fd);
	}
	else if (socket_.isListeningSocket(sock_fd) == true)
	{
		int new_client = socket_.accept(sock_fd);
		setReadEvent(new_client);
	}
	else
		throw std::runtime_error("[EVFILT_READ] : bad socket descriptor");
}

void Kqueue::eventWrite(int sock_fd)
{
	if (socket_.isClientSocket(sock_fd) == true)
		socket_.send(sock_fd);
	else
		throw std::runtime_error("[EVFILT_WRITE] : bad descriptor");
}

void Kqueue::setReadEvent(int sock_fd)
{
	struct kevent change_event;
	EV_SET(&change_event, sock_fd, EVFILT_READ, EV_ADD , 0, 0, NULL);
		if (-1 == kevent(kq_, &change_event, 1, NULL, 0, NULL))
			throw std::runtime_error("[Kqueue] : read event add error");
}

void Kqueue::setWriteEvent(int sock_fd)
{
	struct kevent change_event;
	EV_SET(&change_event, sock_fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT , 0, 0, NULL);
		if (-1 == kevent(kq_, &change_event, 1, NULL, 0, NULL))
			throw std::runtime_error("[Kqueue] : write event add error");
}
