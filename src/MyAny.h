#pragma once
#include <any>
#include <string>

class CMyAny : public std::any
{
public:
    operator std::string() const;

};