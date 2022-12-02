#include "gtest/gtest.h"
#include "delete_method.hpp"
#include "status_code.hpp"

#include <fstream>

TEST(DeleteTest, DeleteFile) {
	std::string uri = "/Users/minjune/webserv/html/test.txt";
	std::ofstream ofs(uri);
	ofs << "test file" << std::endl;

	int actual = DeleteMethod(uri);
	int expected = OK;
	EXPECT_EQ(actual, expected);
}

TEST(DeleteTest, DeleteNotExistFile) {
	std::string uri = "/Users/minjune/webserv/html/hi_bye";
	int actual = DeleteMethod(uri);
	int expected = NOT_FOUND;
	EXPECT_EQ(actual, expected);
}

TEST(DeleteTest, DeleteDirectory) {
	std::string uri = "/Users/minjune/webserv/html/test";
	int actual = DeleteMethod(uri);
	int expected = CONFLICT;
	EXPECT_EQ(actual, expected);
}
