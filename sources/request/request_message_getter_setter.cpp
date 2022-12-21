#include "request_message.hpp"

int	RequestMessage::GetClientMaxBodySize() const {
	return (client_max_body_size_);
}

StatusCode	RequestMessage::GetStatusCode() const {
	return (status_code_);
}

int	RequestMessage::GetContentSize() const {
	return (content_size_);
}

bool	RequestMessage::IsChunked() const {
	return (is_chunked_);
}

bool	RequestMessage::IsAlive() const {
	return (keep_alive_);
}

RequestState	RequestMessage::GetState() const {
	return (state_);
}

const std::string	&RequestMessage::GetTempHeaderName() const
{
	return (temp_header_name_);
}

const std::string	&RequestMessage::GetTempHeaderValue() const
{
	return (temp_header_value_);
}

bool	RequestMessage::IsLastChunk() const {
	return (last_chunk_flag_);
}

size_t	RequestMessage::GetChunkSize() const {
	return (chunk_size_);
}

const std::string	&RequestMessage::GetChunkSizeStr() const {
	return (chunk_size_str_);
}

const std::string	&RequestMessage::GetChunkBody() const {
	return (chunk_body_);
}

const std::string	&RequestMessage::GetMethod() const {
	return (method_);
}

const std::string	&RequestMessage::GetUri() const {
	return (uri_);
}

const std::string	&RequestMessage::GetHttpVersion() const {
	return (http_version_);
}

const RequestMessage::headers_type	&RequestMessage::GetHeaders() const {
	return (headers_);
}

const std::string	&RequestMessage::GetBody() const {
	return (body_);
}


void RequestMessage::SetState(RequestState code) {
	state_ = code;
}

void RequestMessage::SetStatusCode(StatusCode code) {
	status_code_ = code;
}

void RequestMessage::SetConnection(bool is_keep_alive) {
	keep_alive_ = is_keep_alive;
}

void RequestMessage::SetLastChunk(bool is_last_chunk) {
	last_chunk_flag_ = is_last_chunk;
}

void RequestMessage::SetChunkSize(size_t size) {
	chunk_size_ = size;
}

void RequestMessage::ClearChunkSize() {
	chunk_size_ = 0;
}

void RequestMessage::ClearChunkSizeStr() {
	chunk_size_str_ = "";
}

void RequestMessage::ClaerChunkBody() {
	chunk_body_ = "";
}

