#ifndef CONFIG_UTILS_HPP
#define CONFIG_UTILS_HPP

#include <string>
#include <vector>

#include "location_info.hpp"
#include "server_info.hpp"

std::vector<std::string> Split(std::string input, char delimiter);
ServerInfo FindServerInfoToRequestHost(
	const std::string &server_name, const std::vector<ServerInfo> &ServerInfo_);
int FindLocationInfoToUri(const std::string &uri,
						  const ServerInfo &ServerInfo_);

#endif
