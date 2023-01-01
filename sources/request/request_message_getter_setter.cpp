#include "request_message.hpp"

int RequestMessage::GetClientMaxBodySize() const {
	return (client_max_body_size_);
}

int RequestMessage::GetContentSize() const {
	return (content_size_);
}

bool RequestMessage::IsChunked() const {
	return (is_chunked_);
}

bool RequestMessage::ShouldClose() const {
	return (!keep_alive_);
}

RequestState RequestMessage::GetState() const {
	return (state_);
}

const std::string &RequestMessage::GetHeaderValue(const std::string &key) const {
	RequestMessage::headers_type::const_iterator it = headers_.find(key);
	return it->second;
}

const std::string &RequestMessage::GetTempHeaderName() const {
	return (temp_header_name_);
}

const std::string &RequestMessage::GetTempHeaderValue() const {
	return (temp_header_value_);
}

bool RequestMessage::IsLastChunk() const {
	return (last_chunk_flag_);
}

int RequestMessage::GetChunkSize() const {
	return (chunk_size_);
}

const std::string &RequestMessage::GetChunkSizeStr() const {
	return (chunk_size_str_);
}

const std::string &RequestMessage::GetChunkBody() const {
	return (chunk_body_);
}

const std::string &RequestMessage::GetMethod() const {
	return (method_);
}

const std::string &RequestMessage::GetUri() const {
	return (uri_);
}

std::string &RequestMessage::GetUri() {
    return (uri_);
}

const std::string &RequestMessage::GetHttpVersion() const {
	return (http_version_);
}

const RequestMessage::headers_type &RequestMessage::GetHeaders() const {
	return (headers_);
}

const std::string &RequestMessage::GetBody() const {
	return (body_);
}

std::string &RequestMessage::GetBody() {
	return (body_);
}

const std::string &RequestMessage::GetResolvedUri() const {
	return resolved_uri_;
}

const std::string &RequestMessage::GetQuery() const {
	return query_;
}

// 호출 이전에 IsThereHost()를 호출하는 부분이 필요하다.
const std::string &RequestMessage::GetServerName() const {
	return (headers_.at("host"));
}

void RequestMessage::SetClientMaxBodySize(int max_size) {
	client_max_body_size_ = max_size;
}

void RequestMessage::SetState(RequestState code) {
	state_ = code;
}

void RequestMessage::SetChunked(bool is_chunked) {
	is_chunked_ = is_chunked;
}

void RequestMessage::SetContentSize(int size) {
	content_size_ = size;
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

void RequestMessage::SetResolvedUri(const std::string &resolved_uri) {
	resolved_uri_ = resolved_uri;
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

/* TEST 전용 */
void RequestMessage::setUri(const std::string &uri) {
    uri_ = uri;
}

void RequestMessage::SetQuery(const std::string &query) {
    query_ = query;
}
