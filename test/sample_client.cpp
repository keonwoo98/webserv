#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
int main()
{
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		std::cerr << "socket failed" << std::endl;
		return (1);
	}
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(8181);
	int connection_fd = connect(fd, (struct sockaddr *)(&server_addr), sizeof(server_addr));
	if (connection_fd == -1)
	{
		close(fd);
		std::cerr << "connection failed" << std::endl;
		return (1);
	}
	char str[] = "POST / HTTP/1.1\r";
	send(fd, str, sizeof(str), 0);
	sleep(1);

	char str3[] = "\nhost: hi\r\ntransfer-encoding: chunked\r\n\r\n3\r\n12";
	send(fd, str3, sizeof(str3), 0);
	sleep(1);

	char str2[] = "3\r\n8\r\n12345678\r\n0\r\n\r\n";
	send(fd, str2, sizeof(str2), 0);
	sleep(1);
	char temp[1024] = {};
	recv(fd, temp, 1024, 0);
	std::cout << temp << std::endl;
}
