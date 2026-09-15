#ifndef CINTN_H
#define CINTN_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>

#define N 20

class CIntN
{
private:
    unsigned char m_digits[N];

public:
    CIntN();
    CIntN(unsigned long long value);
    CIntN(const char *str);
    CIntN(const CIntN &b);
    ~CIntN();
    CIntN& operator+(const CIntN &b) const;
    CIntN& operator-(const CIntN &b) const;
    int operator==(const CIntN &b) const;
    friend std::ostream& operator<<(std::ostream &out, const CIntN &number);
};

#endif
