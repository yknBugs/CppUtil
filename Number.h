#pragma once

#include <vector>
#include <string>

#define DEFAULT_LENGTH 127

class Number
{
private:
    bool isNegative;
    std::vector<int> primary;
    std::vector<int> decimal;
    size_t decimalLength;

    void adjustDigits();

public:
    Number();
    Number(int n);
    Number(double n);
    Number(std::string n);
    Number(const Number& n);

    Number operator - () const;
    Number& operator = (const Number& n);
    Number operator + (const Number& n) const;
    Number operator - (const Number& n) const;
    // Number operator * (const Number& n) const;
    // Number operator / (const Number& n) const;
    bool operator == (const Number& n) const;
    bool operator != (const Number& n) const;
    bool operator < (const Number& n) const;
    bool operator > (const Number& n) const;
    bool operator <= (const Number& n) const;
    bool operator >= (const Number& n) const;
    
    operator int() const;
    operator double() const;
    operator std::string() const;
};