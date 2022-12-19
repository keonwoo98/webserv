#ifndef URI_HPP
#define URI_HPP

#include <iostream>
#include <map>

class Uri {
   public:
	typedef std::map<std::string, std::string> query_map_type;
	Uri(const std::string &uri);
	~Uri();

	const std::string &GetOriginUri() const;
	const std::string &GetPath() const;
	const query_map_type &GetQueryMap() const;

	void SplitUri();
	std::string ToString();

   private:
	std::string origin_uri_;
	std::string path_;
	std::map<std::string, std::string> query_map_;

	void ApplyConfigToPath();
	void ParsingQueryString(const std::string &query);
};

std::ostream &operator<<(std::ostream &out, Uri uri);
#endif
