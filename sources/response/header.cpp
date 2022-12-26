//
// Created by 김민준 on 2022/12/19.
//

#include "header.hpp"
#include "date.hpp"
#include "character_const.hpp"

#include <sstream>

Header::Header() {}

void Header::Add(const std::pair<std::string, std::string> &header) {
	headers_.insert(header);
}

void Header::AddDate(time_t raw_time) {
	Add(std::make_pair("Date", GetTime(raw_time)));
}

void Header::AddServer() {
	Add(std::make_pair("Server", "Webserv"));
}

void Header::AddContentLength(const std::string &body) {
	std::stringstream ss;

	ss << body.length();
	Add(std::make_pair("Content-Length", ss.str()));
}

void Header::AddContentType(const std::string &type) {
	Add(std::make_pair("Content-Type", type));
}

/**
 * The actual set of allowed methods is defined by the origin server at the time of each request.
 * An origin server MUST generate an Allow header field in a 405 (Method Not Allowed) response and MAY do so in any other response.
 * An empty Allow field value indicates that the resource allows no methods,
 * which might occur in a 405 response if the resource has been temporarily disabled by configuration.
 * @param allow_method
 */
void Header::AddAllow(const std::vector<std::string> &allow_method) {
	std::stringstream ss;
	for (size_t i = 0; i < allow_method.size(); i++) {
		ss << allow_method[i];
		if (i != allow_method.size() - 1) {
			ss << ", ";
		}
	}
	Add(std::make_pair("Allow", ss.str()));
}

/**
 * The "Location" header field is used in some responses to refer to a specific resource in relation to the response.
 * The type of relationship is defined by the combination of request method and status code semantics.
 * The field value consists of a single URI-reference.
 *
 * The Location response header indicates the URL to redirect a page to.
 * It only provides a meaning when served with a 3xx (redirection) or 201 (created) status response.
 *
 * In cases of resource creation 201(Created), it indicates the URL to the newly created resource.
 * Location indicates the target of a redirection or the URL of a newly created resource.
 * @param uri_ref
 */
void Header::AddLocation(const std::string &uri_ref) {
	Add(std::make_pair("Location", uri_ref));
}

/**
 * The Connection general header controls whether the network connection stays open after the current transaction finishes.
 * If the value sent is keep-alive, the connection is persistent and not closed, allowing for subsequent requests to the same server to be done.
 *
 * Keep-Alive
 * This value tells the server to keep the connection open after the response has been sent,
 * so that subsequent requests can be sent over the same connection.
 * This can be more efficient than establishing a new connection for each request.
 *
 * Close
 * This value tells the server to close the connection after the response has been sent.
 * This can be useful if the client does not want to keep the connection open, or if it expects to be idle for an extended period of time.
 *
 * @param connection
 */
void Header::AddConnection(const std::string &connection) {
	Add(std::make_pair("Connection", connection));
}

/**
 * The Transfer-Encoding header specifies the form of encoding used to safely transfer the payload body to the user.
 *
 * example
 * Transfer-Encoding: chunked
 * Transfer-Encoding: compress
 * Transfer-Encoding: deflate
 * Transfer-Encoding: gzip
 *
 * chunked
 * Data is sent in a series of chunks.
 * The Content-Length header is omitted in this case and at the beginning of each chunk
 * you need to add the length of the current chunk in hexadecimal format,
 * followed by '\r\n' and then the chunk itself, followed by another '\r\n'.
 * The terminating chunk is a regular chunk, with the exception that its length is zero.
 * It is followed by the trailer, which consists of a (possibly empty) sequence of header fields.
 *
 * @param transfer_encoding
 */
void Header::AddTransferEncoding(const std::string &transfer_encoding) {
	Add(std::make_pair("Transfer-Encoding", transfer_encoding));
}

std::string Header::ToString() {
	std::stringstream ss;
	headers_type::iterator it = headers_.begin();
	while (it != headers_.end()) {
		ss << it->first << ": " << it->second << CRLF;
		it++;
	}
	ss << CRLF;
	return ss.str();
}

void Header::Clear() {
	headers_.clear();
}

std::ostream &operator<<(std::ostream &out, Header header) {
	out << header.ToString();
	return out;
}
