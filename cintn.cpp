#include "cintn.h"
#include <cstring>

CIntN::CIntN()
{
    for (int i = 0; i < N; i++)
        m_digits[i] = 0;
}

CIntN::CIntN(unsigned long long value)
{
    for (int i = 0; i < N; i++)
    {
        m_digits[i] = value % 10;
        value /= 10;
    }
}

CIntN::CIntN(const char *str)
{
    int len = std::strlen(str);

    for (int i = 0; i < N; i++)
        m_digits[i] = 0;

    int pos = 0;
    for (int i = len - 1; i >= 0 && pos < N; i--, pos++)
    {
        if (str[i] >= '0' && str[i] <= '9')
            m_digits[pos] = str[i] - '0';
    }
}

CIntN::CIntN(const CIntN &b)
{
    for (int i = 0; i < N; i++)
        m_digits[i] = b.m_digits[i];
}

CIntN::~CIntN()
{
}

CIntN CIntN::operator+(const CIntN &b) const
{
    CIntN result;
    int carry = 0;

    for (int i = 0; i < N; i++)
    {
        int sum = m_digits[i] + b.m_digits[i] + carry;
        result.m_digits[i] = sum % 10;
        carry = sum / 10;
    }

    return result;
}

CIntN CIntN::operator-(const CIntN &b) const
{
    CIntN result;
    int borrow = 0;

    for (int i = 0; i < N; i++)
    {
        int difference = (int)m_digits[i] - (int)b.m_digits[i] - borrow;

        if (difference < 0)
        {
            difference += 10;
            borrow = 1;
        }
        else
            borrow = 0;

        result.m_digits[i] = difference;
    }

    return result;
}

bool CIntN::operator==(const CIntN &b) const
{
    for (int i = 0; i < N; i++)
        if (m_digits[i] != b.m_digits[i])
            return false;

    return true;
}

std::ostream& operator<<(std::ostream &out, const CIntN &number)
{
    int i = N - 1;

    while (i > 0 && number.m_digits[i] == 0)
        i--;

    for (; i >= 0; i--)
        out << (int)number.m_digits[i];

    return out;
}
