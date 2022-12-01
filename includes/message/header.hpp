#ifndef HEADER_HPP
#define HEADER_HPP

#include <map>
#include <vector>
#include <iostream>

class Header {
   private:
	std::map<std::string, std::vector<std::string> > headers_;

   public:
	explicit Header();
	virtual ~Header();
	std::string toString() const;
	void Add(std::string &key, std::vector<std::string> &value);
	friend std::ostream &operator<<(std::ostream &os, const Header &header);
};

#endif
