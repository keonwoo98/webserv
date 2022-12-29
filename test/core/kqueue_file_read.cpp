//
// Created by 김민준 on 2022/12/29.
//

#include <sys/event.h>
#include <fcntl.h>
#include <fstream>
#include "gtest/gtest.h"

TEST(Kqueue, FileRead) {
	int kq = kqueue();
	ASSERT_GT(kq, 0);

	int file_fd = open("./test.txt", O_RDWR | O_CREAT, 0644);
	if (file_fd < 0) {
		perror("open");
	}
	ASSERT_GT(file_fd, 0);

	write(file_fd, "Hello World!", strlen("Hello World!"));
	close(file_fd);

	file_fd = open("./test.txt", O_RDONLY | O_CREAT, 0644);
	struct kevent change;
	EV_SET(&change, file_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);

	struct kevent event;
	int num = kevent(kq, &change, 1, &event, 1, NULL);
	EXPECT_EQ(num, 1);
	EXPECT_EQ(event.ident, file_fd);
	EXPECT_EQ(event.filter, EVFILT_READ);
	EXPECT_EQ(event.data, strlen("Hello World!"));

	close(file_fd);
	unlink("./test.txt");
}
