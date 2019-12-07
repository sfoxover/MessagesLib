#include "MyAny.h"

CMyAny::operator std::string() const
{
    std::string result = std::any_cast<std::string>(*this);
    return result;
}