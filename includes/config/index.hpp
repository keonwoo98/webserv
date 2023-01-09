#ifndef INDEX_HPP
#define INDEX_HPP

#include <string>
#include <vector>

class Index {
   public:
	Index();
	Index(std::string &index);
	~Index();

	bool FindIndex(const std::string &index);
	const std::vector<std::string> &GetIndex() const;
	std::string ToString() const;

   private:
	std::vector<std::string> index_;
};

std::ostream &operator<<(std::ostream &out, const Index &index);

#endif
