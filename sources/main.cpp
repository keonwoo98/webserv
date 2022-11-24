#include <vector>
#include <iostream>

#include <Socket.hpp>
#include <Kqueue.hpp>

void webserv(char ** argv);

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "./[prog] [port1] [port2] ..." << std::endl;
		std::exit(1);
	}
	try
	{
		webserv(argv);
	}
	catch (const std::exception & e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "unexpected exception" << std::endl;
	}
	return 0;
}

void webserv(char ** argv)
{
	std::vector<std::string> ports;
	while (*(++argv) != NULL)
		ports.push_back(std::string(*argv));

	Kqueue webserv_kqueue(ports);
	webserv_kqueue.eventLoop();
}
