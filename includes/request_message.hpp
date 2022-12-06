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

	void ParsingMessage(const std::string &message);

   private:
	std::string method_;
	std::string uri_;
	header_map_type header_map_;
	std::string body_;

	void ParsingStartLine(const std::string start_line);
	void ParsingHeaders(const std::string headers);
	void SplitHeader(const std::string &header);
	bool IsInHeaderMap(const std::string &name) const;
	bool TransferEncodingValidation(const std::string &value) const;
	bool TransferEncodingValidation(const std::string &value) const;
	bool IsThereHostInHeaders() const;
	bool HeaderValidation(const RequestMessage::header_map_type::iterator &header) const;
};

#endif
