#include "message/body.hpp"

#include <sstream>

Body::Body(const std::string &entity) : entity_(entity) {}

Body::~Body() {}

std::string Body::toString() const {
	std::stringstream ss;
	ss << entity_;
	return ss.str();
}

std::ostream &operator<<(std::ostream &out, const Body &body) {
	out << body.toString();
	return out;
}