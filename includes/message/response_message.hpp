/**
 * response message
 *
 * HTTP Message = start-line
 * 				*(header-field CRLF)
 * 				CRLF
 * 				[ message-body ]
 *
 *
 * reason-phrase is possibly empty
 *
 * status-code = 3DIGIT
 *
 * header-field = field-name ":" OWS field-value OWS
 * field-name = token
 * field-value = *(field-content / obs-fold)
 * field-content = field-vchar [ 1*(SP / HTAB) field-vchar ]
 * field-vchar = VCHAR / obs-text
 * obs-fold = CRLF 1*(SP / HTAB)
 * 			; obsolete line folding
 *
 * The order in which header fields with differing field names are received is not significant.
 * However, it is good practice to send header fields that contain control data first,
 * such as Host on requests and Date on responses, so that implementations can decide
 * when not to handle a message as early as possible.
 *
 * OWS -> OWS rule is used where zero or more linear whitespace octets might appear.
 *
 * No whitespace is allowed between the header field-name and colon.
 * A server MUST reject any received request message that contains whitespace between a header field-name and colon
 * with a response code of 400 (Bad Request)
 *
 * message-body = *OCTET
 */
#ifndef RESPONSE_MESSAGE_HPP
#define RESPONSE_MESSAGE_HPP

#include <string>
#include <ostream>

#include "body.hpp"
#include "header.hpp"
#include "status_line.hpp"

class ResponseMessage {
   private:
	StatusLine status_line;
	Header header_;
	Body body_;

   public:
	ResponseMessage(const StatusLine &status_line, const Header &header, const Body &body);
	virtual ~ResponseMessage();
	std::string toString() const;
};

std::ostream &operator<<(std::ostream &os, const ResponseMessage &message);
#endif