#ifndef START_LINE_HPP
#define START_LINE_HPP

#include <string>
#include <ostream>

#include "http_version.hpp"

class StatusLine {
   private:
	HttpVersion version_;
	int status_code_;
	std::string reason_phrase_;

   public:
	StatusLine(HttpVersion version, int status_code, std::string reason_phrase);
	virtual ~StatusLine();
	std::string toString() const;
};

std::ostream &operator<<(std::ostream &os, const StatusLine &status_line);

#endif
