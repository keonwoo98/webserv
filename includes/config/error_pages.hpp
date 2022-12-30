#ifndef ERROR_PAGES_HPP
#define ERROR_PAGES_HPP

#include <map>
#include <string>

class ErrorPages {
   public:
	typedef std::map<int, std::string> error_page_type;
	ErrorPages();
	~ErrorPages();
	void Append(std::string &error_pages);
	const std::string GetPath(int &num) const;
	const error_page_type &GetErrorPages() const;
	std::string ToString() const;

   private:
	error_page_type error_pages_;
};

std::ostream &operator<<(std::ostream &out, const ErrorPages &error_pages);

#endif
