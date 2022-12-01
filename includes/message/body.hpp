#ifndef BODY_HPP
#define BODY_HPP

#include <string>
#include <iostream>

class Body {
   private:
	std::string entity_;

   public:
	explicit Body(const std::string &entity);
	virtual ~Body();
	std::string toString() const;
};

std::ostream &operator<<(std::ostream &out, const Body &body);

#endif
