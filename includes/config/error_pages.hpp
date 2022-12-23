#ifndef ERROR_PAGES_HPP
#define ERROR_PAGES_HPP

#include <map>
#include <string>

class ErrorPages {
   private:
	std::map<int, std::string> error_pages_;

   public:
	ErrorPages();
	~ErrorPages();
	void Append(std::string &error_pages);
	const std::string GetPath(int &num) const;
	const std::map<int, std::string> &GetErrorPages() const;
};

#endif
