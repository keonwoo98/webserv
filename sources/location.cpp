#include "location.hpp"

Location::Location() : client_max_body_size_(1000000), path_(""), root_("./docs/index.html") {}

Location::~Location() {}
