#include "general/string_exception.h"

string_exception::string_exception(std::string _w)
{
    _what = _w;
}

string_exception::~string_exception() throw()
{
    //dtor
}

const char* string_exception::what() const throw()
{
    return _what.c_str();
}
