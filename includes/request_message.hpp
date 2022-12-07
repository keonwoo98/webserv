#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include <iostream>
#include <map>
#include <set>

#define CRLF "\r\n"
#define DCRLF "\r\n\r\n"
#define CR '\r'
#define LF '\n'
#define SP ' '
#define HT '\t'

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
	size_t	content_size_;
	bool	is_chunked_;
	bool	keep_alive_;


	std::set<std::string> valid_header_name_;
	std::set<std::string> valid_header_value_;

	void ParsingStartLine(const std::string start_line);
	void ParsingHeaders(const std::string headers);
	void SplitHeader(const std::string &header);
	void ParsingBody(const std::string &body);

	bool isTchar(char c);
};

#endif
