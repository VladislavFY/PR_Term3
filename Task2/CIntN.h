#ifndef CINTN_H
#define CINTN_H

#include <iostream>

class CIntN
{
private:
    int m_n;
    unsigned char *m_digits;
    bool m_negative;

    bool is_zero() const;
    int compare_abs(const CIntN &b) const;

public:
    CIntN(int n = 1);
    CIntN(int n, const char *str);
    CIntN(const CIntN &b);
    CIntN(CIntN &&b);
    ~CIntN();

    CIntN& operator=(const CIntN &b);
    CIntN& operator=(CIntN &&b);

    CIntN operator+(const CIntN &b) const;
    CIntN operator-(const CIntN &b) const;

    friend std::ostream& operator<<(std::ostream &out, const CIntN &number);
};

#endif
