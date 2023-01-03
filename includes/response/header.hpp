//
// Created by 김민준 on 2022/12/19.
//

#ifndef WEBSERV_INCLUDES_HEADER_HPP
#define WEBSERV_INCLUDES_HEADER_HPP

#include <map>
#include <vector>

class Header {
   public:
	typedef std::map<std::string, std::string> headers_type;

	Header();

	void Add(const std::string &key, const std::string &value);
	void AddDate(time_t i);
	void AddServer();
	void AddContentLength(int length);
	void AddAllow(const std::vector<std::string> &allow_method);
	void AddContentType(const std::string &type);
	void AddLocation(const std::string &uri_ref);
	void AddConnection(const std::string &connection);
	void AddTransferEncoding(const std::string &transfer_encoding);

	std::string ToString();

	void Clear();

   private:
	void Add(const std::pair<std::string, std::string> &header);
	headers_type headers_;
};

std::ostream &operator<<(std::ostream &out, Header header);

#endif	// WEBSERV_INCLUDES_HEADER_H_
