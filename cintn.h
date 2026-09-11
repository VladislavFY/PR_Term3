#ifndef CINTN_H
#define CINTN_H

#include <iostream>

#define N 20

class CIntN
{
private:
    unsigned char m_digits[N];

public:
    // default constructor
    CIntN();

    // constructor from unsigned integer
    CIntN(unsigned long long value);

    // constructor from decimal string
    CIntN(const char *str);

    // copy constructor
    CIntN(const CIntN &b);

    // destructor
    ~CIntN();

    // operator +
    CIntN operator+(const CIntN &b) const;

    // operator -
    CIntN operator-(const CIntN &b) const;

    // operator ==, useful for tests
    bool operator==(const CIntN &b) const;

    // output operator
    friend std::ostream& operator<<(std::ostream &out, const CIntN &number);
};

#endif
