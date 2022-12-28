#include "gtest/gtest.h"
#include "delete_method.hpp"
#include "status_code.hpp"

#include <fstream>

namespace {

TEST(DeleteTest, DeleteFile) {
	std::string uri = "./docs/test.txt";
	std::ofstream ofs(uri); // create test.txt file
	ofs << "test file" << std::endl;

	int actual = DeleteMethod(uri);
	int expected = OK;
	EXPECT_EQ(actual, expected);
}

TEST(DeleteTest, DeleteNotExistFile) {
	std::string uri = "./docs/42_hello.html";
	int actual = DeleteMethod(uri);
	int expected = NOT_FOUND;
	EXPECT_EQ(actual, expected);
}

TEST(DeleteTest, DeleteDirectory) {
	std::string uri = "./docs/cgi-bin";
	int actual = DeleteMethod(uri);
	int expected = CONFLICT;
	EXPECT_EQ(actual, expected);
}
}
