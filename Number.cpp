#include "Number.h"

void Number::adjustDigits()
{
    if (this->primary.size() == 0)
    {
        this->primary.push_back(0);
    }
    // Adjust the decimal part
    if (this->decimal.size() > 0)
    {
        for (size_t i = this->decimal.size() - 1; i > 0; i--)
        {
            if (this->decimal[i] < 0)
            {
                this->decimal[i - 1] -= ((-this->decimal[i]) / 10 + 1);
                this->decimal[i] = (10 - ((-this->decimal[i]) % 10));
            }
            if (this->decimal[i] >= 10)
            {
                this->decimal[i - 1] += (this->decimal[i] / 10);
                this->decimal[i] = (this->decimal[i] % 10);
            }
        }
        // Adjust decimal point
        if (this->decimal[0] < 0)
        {
            this->primary[0] -= ((-this->decimal[0]) / 10 + 1);
            this->decimal[0] = (10 - ((-this->decimal[0]) % 10));
        }
        if (this->decimal[0] >= 10)
        {
            this->primary[0] += (this->decimal[0] / 10);
            this->decimal[0] = (this->decimal[0] % 10);
        }
    }
    // Adjust primary part
    for (size_t i = this->primary.size() - 1; i > 0; i--)
    {
        if (this->primary[i] < 0)
        {
            this->primary[i - 1] -= ((-this->primary[i]) / 10 + 1);
            this->primary[i] = (10 - ((-this->primary[i]) % 10));
        }
        if (this->primary[i] >= 10)
        {
            this->primary[i - 1] += (this->primary[i] / 10);
            this->primary[i] = (this->primary[i] % 10);
        }
    }
    // extend the primary part if necessary
    while (this->primary[0] >= 10)
    {
        this->primary.insert(this->primary.begin(), this->primary[0] / 10);
        this->primary[1] = (this->primary[1] % 10);
    }
    while (this->primary[0] <= -10)
    {
        this->primary.insert(this->primary.begin(), this->primary[0] / 10);
        this->primary[1] = (this->primary[1] % 10);
        if (this->primary[1] < 0) {
            this->primary[0] -= 1;
            this->primary[1] += 10;
        }
    }
    // adjust the negative sign if necessary
    if (this->primary[0] < 0) {
        this->isNegative = (!this->isNegative);
        this->primary[0] *= -1;
        for (size_t i = 1; i < this->primary.size(); i++)
        {
            this->primary[i] = 10 - this->primary[i];
            this->primary[i - 1] -= 1;
        }
        if (this->decimal.size() > 0)
        {
            this->decimal[0] = 10 - this->decimal[0];
            this->primary[0] -= 1;
        }
        for (size_t i = 1; i < this->decimal.size(); i++)
        {
            this->decimal[i] = 10 - this->decimal[i];
            this->decimal[i - 1] -= 1;
        }
    }
    // remove leading zeros and ending zeros in decimal part
    while (this->primary.size() > 1 && this->primary[0] == 0)
    {
        this->primary.erase(this->primary.begin());
    }
    while (this->decimal.size() > 0 && this->decimal[this->decimal.size() - 1] == 0)
    {
        this->decimal.pop_back();
    }
}

Number::Number()
{
    this->isNegative = false;
    this->decimalLength = DEFAULT_LENGTH;
    this->primary.push_back(0);
}

Number::Number(int n)
{
    this->decimalLength = DEFAULT_LENGTH;
    if (n < 0)
    {
        this->isNegative = true;
        n *= -1;
    }
    else
    {
        this->isNegative = false;
    }

    while (n > 0)
    {
        int a = n % 10;
        this->primary.insert(this->primary.begin(), a);
        n /= 10;
    }
}

Number::Number(double n)
{
    this->decimalLength = DEFAULT_LENGTH;
    if (n < 0)
    {
        this->isNegative = true;
        n *= -1;
    }
    else
    {
        this->isNegative = false;
    }

    int integerPart = static_cast<int>(n);
    double decimalPart = n - integerPart;

    while (integerPart > 0)
    {
        int a = integerPart % 10;
        this->primary.insert(this->primary.begin(), a);
        integerPart /= 10;
    }

    while (this->decimal.size() < this->decimalLength && decimalPart > 0)
    {
        decimalPart *= 10;
        int a = static_cast<int>(decimalPart);
        this->decimal.push_back(a);
        decimalPart -= a;
    }
}

Number::Number(std::string n)
{
    if (n[0] == '-')
    {
        this->isNegative = true;
        n = n.substr(1);
    }
    else
    {
        this->isNegative = false;
    }
    bool isDecimal = false;
    for (size_t i = 0; i < n.size(); i++)
    {
        if (n[i] >= '0' && n[i] <= '9')
        {
            if (isDecimal)
            {
                this->decimal.push_back(n[i] - '0');
            }
            else
            {
                this->primary.push_back(n[i] - '0');
            }
        }
        else if (n[i] == '.')
        {
            isDecimal = true;
        }
    }
    this->decimalLength = this->decimal.size() > DEFAULT_LENGTH ? this->decimal.size() : DEFAULT_LENGTH;
    this->adjustDigits();
}

Number::Number(const Number& n)
{
    this->isNegative = n.isNegative;
    this->primary = n.primary;
    this->decimal = n.decimal;
    this->decimalLength = n.decimalLength;
}

Number& Number::operator = (const Number& n)
{
    this->isNegative = n.isNegative;
    this->primary = n.primary;
    this->decimal = n.decimal;
    this->decimalLength = n.decimalLength;
    return *this;
}

Number Number::operator - () const
{
    Number result = *this;
    result.isNegative = (!result.isNegative);
    return result;
}

Number Number::operator + (const Number& n) const
{
    Number result;
    result.primary.clear();
    result.decimalLength = this->decimalLength > n.decimalLength ? this->decimalLength : n.decimalLength;

    if (this->isNegative == n.isNegative)
    {
        result.isNegative = this->isNegative;
        // Add primary part
        for (size_t i = 0; i < n.primary.size() || i < this->primary.size(); i++)
        {
            int a = i < this->primary.size() ? this->primary[this->primary.size() - i - 1] : 0;
            int b = i < n.primary.size() ? n.primary[n.primary.size() - i - 1] : 0;
            result.primary.insert(result.primary.begin(), a + b);
        }
        // Add decimal part
        for (size_t i = 0; i < n.decimal.size() || i < this->decimal.size(); i++)
        {
            int a = i < this->decimal.size() ? this->decimal[i] : 0;
            int b = i < n.decimal.size() ? n.decimal[i] : 0;
            result.decimal.push_back(a + b);
        }
        result.adjustDigits();
    }
    else if (this->isNegative == false && n.isNegative == true)
    {
        result = (*this) - (-n);
    }
    else
    {
        result = n - (-(*this));
    }


    return result;
}

Number Number::operator - (const Number& n) const 
{
    Number result;
    result.primary.clear();
    result.decimalLength = this->decimalLength > n.decimalLength ? this->decimalLength : n.decimalLength;

    if (this->isNegative == false && n.isNegative == false)
    {
        if ((*this) >= n)
        {
            result.isNegative = false;
            // Minus primary part
            for (size_t i = 0; i < n.primary.size() || i < this->primary.size(); i++)
            {
                int a = i < this->primary.size() ? this->primary[this->primary.size() - i - 1] : 0;
                int b = i < n.primary.size() ? n.primary[n.primary.size() - i - 1] : 0;
                result.primary.insert(result.primary.begin(), a - b);
            }
            // Minus decimal part
            for (size_t i = 0; i < n.decimal.size() || i < this->decimal.size(); i++)
            {
                int a = i < this->decimal.size() ? this->decimal[i] : 0;
                int b = i < n.decimal.size() ? n.decimal[i] : 0;
                result.decimal.push_back(a - b);
            }
            result.adjustDigits();
        }
        else
        {
            result = n - (*this);
            result.isNegative = true;
        }
    }
    else if (this->isNegative == true && n.isNegative == true)
    {
        result = (-n) - (-(*this));
    }
    else if (this->isNegative == false && n.isNegative == true)
    {
        result = (*this) + (-n);
    }
    else
    {
        result = (-n) + (*this);
    }

    return result;
}

bool Number::operator == (const Number& n) const
{
    if (this->isNegative != n.isNegative)
    {
        return false;
    }
    for (size_t i = 0; i < n.primary.size() || i < this->primary.size(); i++)
    {
        int a = i < this->primary.size() ? this->primary[this->primary.size() - i - 1] : 0;
        int b = i < n.primary.size() ? n.primary[n.primary.size() - i - 1] : 0;
        if (a != b)
        {
            return false;
        }
    }
    for (size_t i = 0; i < n.decimal.size() || i < this->decimal.size(); i++)
    {
        int a = i < this->decimal.size() ? this->decimal[i] : 0;
        int b = i < n.decimal.size() ? n.decimal[i] : 0;
        if (a != b)
        {
            return false;
        }
    }
    return true;
}

bool Number::operator != (const Number& n) const
{
    return !((*this) == n);
}

bool Number::operator < (const Number& n) const
{
    if (this->isNegative == false && n.isNegative == false)
    {
        size_t primarySize = this->primary.size() > n.primary.size() ? this->primary.size() : n.primary.size();
        // compare primary part
        for (size_t i = primarySize - 1; true; i--)
        {
            int a = i < this->primary.size() ? this->primary[this->primary.size() - i - 1] : 0;
            int b = i < n.primary.size() ? n.primary[n.primary.size() - i - 1] : 0;
            if (a < b)
            {
                return true;
            }
            else if (a > b)
            {
                return false;
            }
            if (i == 0)
            {
                break;
            }
        }
        // compare decimal part
        for (size_t i = 0; i < this->decimal.size() || i < n.decimal.size(); i++)
        {
            int a = i < this->decimal.size() ? this->decimal[i] : 0;
            int b = i < n.decimal.size() ? n.decimal[i] : 0;
            if (a < b)
            {
                return true;
            }
            else if (a > b)
            {
                return false;
            }
        }
        // equal
        return false;
    }
    else if (this->isNegative == true && n.isNegative == true)
    {
        return (-n) < (-(*this));
    }
    else if (this->isNegative == false && n.isNegative == true)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Number::operator > (const Number& n) const
{
    return n < (*this);
}

bool Number::operator <= (const Number& n) const
{
    return !(n < (*this));
}

bool Number::operator >= (const Number& n) const
{
    return !((*this) < n);
}

Number::operator int() const
{
    int result = 0;
    for (size_t i = 0; i < this->primary.size(); i++)
    {
        if (result * 10 + this->primary[i] < result)
        {
            // int overflow
            result = INT_MAX;
            break;
        }
        result = result * 10 + this->primary[i];
    }
    return this->isNegative ? -result : result;
}

Number::operator double() const
{
    double result = 0;
    for (size_t i = 0; i < this->primary.size(); i++)
    {
        result = result * 10 + this->primary[i];
    }
    double decimalPart = 1.0;
    for (size_t i = 0; i < this->decimal.size(); i++)
    {
        decimalPart /= 10;
        result += (this->decimal[i] * decimalPart);
    }
    return this->isNegative ? -result : result;
}

Number::operator std::string() const
{
    std::string result;
    if (this->isNegative)
    {
        result += "-";
    }
    for (size_t i = 0; i < this->primary.size(); i++)
    {
        result += std::to_string(this->primary[i]);
    }
    if (this->decimal.size() > 0)
    {
        result += ".";
        for (size_t i = 0; i < this->decimal.size(); i++)
        {
            result += std::to_string(this->decimal[i]);
        }
    }
    return result;
}