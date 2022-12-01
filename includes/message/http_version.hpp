/**
 * HTTP version
 * form : <major>.<minor>
 * case-sensitive
 * HTTP-version = HTTP-name "/" DIGIT "." DIGIT
 * HTTP-name = %x48.54.54.50; "HTTP", case-sensitive
 */

#ifndef HTTP_VERSION_HPP
#define HTTP_VERSION_HPP

#include <iostream>

class HttpVersion {
   private:
	int major_;
	int minor_;

   public:
	HttpVersion(int major, int minor);
	HttpVersion(const HttpVersion &rhs);
	HttpVersion &operator=(const HttpVersion &rhs);
	~HttpVersion();
	std::string toString() const;
};

std::ostream &operator<<(std::ostream &os, const HttpVersion &version);
#endif
