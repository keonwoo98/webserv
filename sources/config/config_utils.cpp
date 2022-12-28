#include "config_utils.hpp"
#include "config_parser.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

std::vector<std::string> Split(std::string input, char delimiter) {
	std::vector<std::string> str;
	std::stringstream ss(input);
	std::string temp;

	while (std::getline(ss, temp, delimiter)) str.push_back(temp);

	return str;
}
// Server 서버 네임이 중복이 안된다는 가정하에
// 이 함수의 리턴 값을 FindLocationInfoToUri() 의 두번째 인자에 넣어준다.
// const ServerInfo &FindServerInfoToRequestHost(
ConfigParser::server_infos_type::const_iterator FindServerInfoToRequestHost(
	const std::string &server_name,
	const std::vector<ServerInfo> &server_infos) {

	ConfigParser::server_infos_type::const_iterator target_it = server_infos.begin();
	for (; target_it != server_infos.end(); target_it++) {
		for (size_t i = 0; i < target_it->GetServerName().size(); i++) {
			if (target_it->GetServerName()[i] == server_name)
				return (target_it);
		}
	}
	return (server_infos.begin());
}

int FindLocationInfoToUri(const std::string &uri,
						  const ServerInfo &ServerInfo_) {
	// uri parsing '/' 기준으로
	// 근데 우리 Split 쓰면 첫번째가 "" 이므로 첫번째 제외하고 벡터에
	// 넣어줘야한다.
	std::vector<std::string> temp_uri_vec = Split(uri, '/');
	std::vector<std::string> uri_vec;
	std::vector<std::vector<std::string> > location_path_vec;
	std::vector<std::string> location_vec;
	std::vector<std::string> temp_location_vec;
	for (size_t i = 1; i <temp_uri_vec.size(); i++)
		uri_vec.push_back(temp_uri_vec[i]);
	for (size_t i = 0; i < ServerInfo_.GetLocations().size(); i++) {
		temp_location_vec = Split(ServerInfo_.GetLocations()[i].GetPath(), '/');
		location_vec.clear();
		for (size_t i = 1; i < temp_location_vec.size(); i++)
			location_vec.push_back(temp_location_vec[i]);
		location_path_vec.push_back(location_vec);
	}
	int priority_vec_size = location_path_vec.size();
	std::vector<int> priority_vec(priority_vec_size);
	for (size_t i = 0; i < location_path_vec.size(); i++) {
		if (uri_vec.size() < location_path_vec[i].size()) {
			priority_vec[i] = -1;
		} else {
			for (size_t j = 0; j < location_path_vec[i].size(); j++) {
				if (uri_vec[j] == location_path_vec[i][j]) priority_vec[i]++;
			}
		}
	}
	int max = max_element(priority_vec.begin(), priority_vec.end()) -
			  priority_vec.begin();
	if (priority_vec[max] <= 0) {
		if (location_path_vec[max].size() != 0) max = -1;
	}
	return max;
}
