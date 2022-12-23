#ifndef INDEX_HPP
#define INDEX_HPP

#include <string>
#include <vector>

class Index {
   private:
	std::vector<std::string> index_;

   public:
	Index();
	Index(std::string &index);
	~Index();
	bool FindIndex(const std::string &index);
	const std::vector<std::string> &GetIndex() const;
};

#endif
