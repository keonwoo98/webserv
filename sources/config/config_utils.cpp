#include "config_utils.hpp"

#include <sstream>
#include <iostream>
#include <algorithm>

std::vector<std::string> Split(std::string input, char delimiter) {
	std::vector<std::string> str;
	std::stringstream ss(input);
	std::string temp;

	while (std::getline(ss, temp, delimiter)) str.push_back(temp);

	return str;
}
// Server 서버 네임이 중복이 안된다는 가정하에
// 이함수의 리턴 값을 FindLocationInfoToUri() 의 두번째 인자에 넣어준다. 
ServerInfo FindServerInfoToRequestHost(const std::string &server_name, const std::vector<ServerInfo> &ServerInfo_) {
	// std::vector<ServerInfo> temp;
	if (ServerInfo_.size() == 1)
		return ServerInfo_[0];
	for (size_t i = 0; i < ServerInfo_.size(); i++) {
		for (size_t j =0; j < ServerInfo_[i].GetServerName().size(); j++){
			if (ServerInfo_[i].GetServerName()[j] == server_name) {
				return ServerInfo_[i];
			}
		}
	}
	// host 에서 준 server_name 이 없으면 ? ServerInfo 의 첫번째 블락 리턴
	return ServerInfo_[0];
}

// 이거 그냥 객체 반환해도 되지않아?
// uri / 일때 는 어떻게 되는거지? 
// / 
// /a
// /a/b
// /a/b/c
int FindLocationInfoToUri(const std::string &uri, const ServerInfo &ServerInfo_) {
	// uri parsing '/' 기준으로 
	// 근데 우리 Split 쓰면 첫번째가 "" 이므로 첫번째 제외하고 벡터에 넣어줘야한다.
	std::vector<std::string> temp_uri_vec = Split(uri, '/');
	std::vector<std::string> uri_vec;
	// for (size_t i = 1; i <temp_uri_vec.size(); i++)
	// 	uri_vec.push_back(temp_uri_vec[i]);
	// std::cout << "uri_vec : [ ";
	// for (size_t i = 0; i < uri_vec.size(); i++) {
	// 	std:: cout << uri_vec[i] << " ";
	// }
	// std::cout << "]" << std::endl; 
	std::vector<std::vector<std::string> > location_path_vec;
	std::vector<std::string> location_vec;
	std::vector<std::string> temp_location_vec;
	for (size_t i = 0; i < ServerInfo_.GetLocations().size(); i++) {
		temp_location_vec = Split(ServerInfo_.GetLocations()[i].GetPath(),'/');
		location_vec.clear();
		for (size_t i = 1; i <temp_location_vec.size(); i++)
			location_vec.push_back(temp_location_vec[i]);
		location_path_vec.push_back(location_vec);
	}
	// std::cout << "location_path_vec : [ ";
	// for (size_t i = 0; i < location_path_vec.size(); i++) {
	// 	std::cout << " [ ";
	// 	for (size_t j =0; j < location_path_vec[i].size(); j++) {
	// 		std:: cout << location_path_vec[i][j] << " ";
	// 	}
	// 	std::cout << "]";
	// }
	// std::cout << "] " << std::endl;

	int priority_vec_size = location_path_vec.size();
	std::vector<int> priority_vec(priority_vec_size);
	for (size_t i = 0; i < location_path_vec.size(); i++) {
		if (uri_vec.size() < location_path_vec[i].size()) {
			priority_vec[i] = -1;
		}
		else {
			for (size_t j =0; j < location_path_vec[i].size(); j++) {
				if (uri_vec[j] == location_path_vec[i][j] )
					priority_vec[i]++;
			}
		}

	}
	// std::cout << " [";  
	// for (size_t j =0; j < priority_vec.size(); j++) {
	// 	std::cout <<  priority_vec[j] << " ";
	// }
	// std::cout << "] "<< std::endl;

	int max = max_element(priority_vec.begin(), priority_vec.end()) - priority_vec.begin();
	// std::cout << priority_vec[max] << " : " << max << std::endl;
	if (priority_vec[max] <= 0) {
		if (location_path_vec[max].size() != 0)
			max = -1;
	}
	// std::cout << max << std::endl;
	return max;
}
