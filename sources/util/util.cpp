#include "util.hpp"

std::string int_to_str(int a) {
	std::stringstream ss;
	ss << a;
	return ss.str();
}
