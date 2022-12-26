//
// Created by Alvin Lee on 2022/12/23.
//

#include "resolve_uri.hpp"
#include "client_socket.hpp"

#include <unistd.h>
#include <iostream>

void Resolve_URI(const ClientSocket &client, const RequestMessage &request, Udata *user_data) {
	std::string uri = request.GetUri();
	ServerInfo server_infos = client.GetServerInfo();
	int location_idx = client.GetLocationIndex();
	std::string root;
	std::string default_root("/var/www");
	std::string base_uri;
	std::vector<std::string> index;
	std::vector<std::string> resolved_uri;
	LocationInfo location_info;

	if (server_infos.IsRoot()) { // root 존재
		base_uri.append(server_infos.GetRoot());
	} else // root 존재 안함
		base_uri.append(default_root);
	if (location_idx == -1) {
		if (server_infos.IsIndex() && uri.compare("/") == 0) { // index 존재 하는 경우 and uri '/'
			index = server_infos.GetIndex();
			for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); ++it) {
				resolved_uri.push_back(base_uri + '/' + *it);
			}
		} else { // index 존재 하지 않는 경우
			base_uri.append(uri);
			resolved_uri.push_back(base_uri);
		}
	} else {
		location_info = server_infos.GetLocations().at(location_idx);
		if (location_info.IsRoot()) { // root 존재
			base_uri.clear();
			base_uri.append(location_info.GetRoot());
		}
		if (location_info.IsIndex() &&
			uri.compare(location_info.GetPath()) == 0) { // index 존재 하는 경우 and uri '/location'
			base_uri.append(location_info.GetPath());
			index = location_info.GetIndex();
			for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); ++it) {
				resolved_uri.push_back(base_uri + '/' + *it);
			}
		} else {
			base_uri.append(uri);
			resolved_uri.push_back(base_uri);
		}
	}
	user_data->request_message_.SetResolvedUri(resolved_uri);
}
