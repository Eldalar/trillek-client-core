#ifndef STRING_EXCEPTION_H
#define STRING_EXCEPTION_H
#include <exception>
#include <string>


class string_exception
    : public std::exception
{
    public:
        string_exception(std::string _w);
        virtual ~string_exception()throw();
        virtual const char* what() const throw();
    protected:
    private:
        std::string _what;
};

#endif // STRING_EXCEPTION_H
