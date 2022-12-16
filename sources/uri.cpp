#include "uri.hpp"

#include <sstream>

Uri::Uri(const std::string &uri) : origin_uri_(uri) { SplitUri(); }

Uri::~Uri() {}

const std::string &Uri::GetOriginUri() const { return origin_uri_; }

const std::string &Uri::GetPath() const { return path_; }

const Uri::query_map_type &Uri::GetQueryMap() const {
	return query_map_;
}

void Uri::SplitUri() {
	size_t index = origin_uri_.find("?");
	if (index == std::string::npos) {
		path_ = origin_uri_;
	} else {
		path_ = origin_uri_.substr(0, index);
		std::string query =
			origin_uri_.substr(index + 1, origin_uri_.length() - (index + 1));
		ParsingQueryString(query);
	}
	ApplyConfigToPath();
}

void Uri::ApplyConfigToPath() {
	path_.insert(0, "/Users/zhy2on/Documents/www");
}

void Uri::ParsingQueryString(const std::string &query) {
	/* ex) "name=John&age=20&city=New+York" */
	// Create a string stream from the query string
	std::stringstream ss(query);

	// Extract the key-value pairs from the stream
	std::string key, value;
	while (std::getline(ss, key, '=') && std::getline(ss, value, '&'))
		query_map_[key] = value;
}
