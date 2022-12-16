#include "gtest/gtest.h"
#include "socket.hpp"

TEST(socket_teest, socket_creation_test) {
	ASSERT_NO_THROW(Socket *socket = new Socket());
}


int main(int ac, char **av) {
	::testing::InitGoogleTest(&ac, av);	
	return RUN_ALL_TESTS();
}
