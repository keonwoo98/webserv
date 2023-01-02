#ifndef ALLOW_METHODS_HPP
#define ALLOW_METHODS_HPP

#include <string>
#include <vector>

class AllowMethods {
    public:
        AllowMethods();
        ~AllowMethods();
        bool IsAllowedMethod(const std::string &x) const;
        void Append(std::string &x);
        std::string ToString() const;
        const std::vector<std::string> &GetAllowMethods() const;
    private:
        std::vector<std::string> allow_methods_;
};
std::ostream &operator<<(std::ostream &out, const AllowMethods &x);

#endif
