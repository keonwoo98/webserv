#include "gtest/gtest.h"
#include "socket.hpp"

TEST(socket_teest, socket_creation_test) {
	ASSERT_NO_THROW(Socket *socket = new Socket());
}