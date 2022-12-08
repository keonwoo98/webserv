#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include <iostream>
#include <map>
#include <set>

class RequestMessage {
   public:
	typedef std::map<std::string, std::string> header_map_type;

	RequestMessage();
	~RequestMessage();

	const std::string &GetMethod() const;
	const std::string &GetUri() const;
	const header_map_type &GetHeaderMap() const;
	const std::string &GetBody() const;

	void SetMethod(const std::string &method);
	void SetUri(const std::string &uri);
	void SetHeader(const std::pair<std::string, std::string> &header);
	void SetBody(const std::string &body);

	bool IsThereHost() const;

   private:
	std::string method_;
	std::string uri_;
	header_map_type header_map_;
	std::string body_;

	void CheckMethod(const std::string &method) const;
	void CheckUri(const std::string &uri) const;
	void CheckHeader(const std::pair<std::string, std::string> &elem) const;
};

#endif
