#include "allow_methods.hpp"
#include "config_utils.hpp"
#include <sstream>
AllowMethods::AllowMethods() {}

AllowMethods::~AllowMethods() {}

void AllowMethods::Append(std::string &x) {
    std::vector<std::string> temp = Split(x, ' ');
    for (size_t i = 0; i < temp.size(); i++) {
    if (temp[i] != "GET" && temp[i] != "POST" && temp[i] != "DELETE")
        allow_methods_.push_back("INVAILD");
    else
        allow_methods_.push_back(temp[i]);
    }
}

bool AllowMethods::IsAllowedMethod(const std::string &x) const{
    for (std::vector<std::string>::const_iterator it = allow_methods_.begin();
		 it != allow_methods_.end(); it++) {
		if (*it == x) return true;
	}
    return false;
}
const std::vector<std::string> &AllowMethods::GetAllowMethods() const {
    return this->allow_methods_;
}

std::string AllowMethods::ToString() const {
	std::stringstream ss;

    ss << "LocationInfo allow method vector is : [ ";
	for (std::vector<std::string>::const_iterator it = allow_methods_.begin();
		 it != allow_methods_.end(); it++) {
		ss << *it  << " ";
	}
    ss << " ]" << '\n';
	return ss.str();
}

std::ostream &operator<<(std::ostream &out, const AllowMethods &x) {
	out << x.ToString();
	return out;
}
