#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include "character_const.hpp"
#include "request_message.hpp"
#include "request_parsing_state.hpp"
#include "server_info.hpp"

/* UTILS */
bool isToken(char c);
bool isVChar(char c);
size_t hexstrToDec(std::string hex_string);

/* PARSER */
void ParseRequest(RequestMessage &req_msg, 
					const std::vector<ServerInfo> &server_infos,
					const char *input);
size_t ParseChunkedRequest(RequestMessage & req_msg, const char * input);

// TODO : 위가 develop에 있던 부분인데, 선택해야함.
/* CHECKER */
// void CheckProtocol(RequestMessage& req_msg, const std::string& protocol);
// bool CheckHeaderField(const RequestMessage& req_msg);
// bool IsThereHost(const RequestMessage& req_msg);
// bool CheckSingleHeaderName(const RequestMessage& req_msg);
// bool RequestStartlineCheck(RequestMessage& req_msg,
// 						   const ServerInfo& server_info);
// bool RequestHeaderCheck(RequestMessage& req_msg, const ServerInfo& server_info);

/* CHECKER */
void CheckProtocol(RequestMessage & req_msg, const std::string & protocol);
bool CheckSingleHeaderName(const RequestMessage & req_msg);
void CheckRequest(RequestMessage & req_msg, const std::vector<ServerInfo> &server_infos);

bool CheckHeaderField(const RequestMessage & req_msg);

#endif
