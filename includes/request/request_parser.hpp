#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include "character_const.hpp"
#include "request_message.hpp"
#include "request_parsing_state.hpp"
#include "server_info.hpp"
#include "client_socket.hpp"

/* UTILS */
bool isToken(char c);
bool isVChar(char c);
size_t hexstrToDec(std::string hex_string);

/* PARSER */
void ParseRequest(RequestMessage &req_msg, 
					ClientSocket *client_socket,
					const ConfigParser::server_infos_type &server_infos,
					const char *input, std::size_t recv_len);
size_t ParseChunkedRequest(RequestMessage & req_msg, const char * input);

/* CHECKER */
void CheckProtocol(RequestMessage & req_msg, const std::string & protocol);
void CheckSingleHeaderName(RequestMessage & req_msg);
void CheckRequest(RequestMessage & req_msg, ClientSocket *client_socket,
					const ConfigParser::server_infos_type& server_infos);

bool CheckHeaderField(const RequestMessage & req_msg);

#endif
