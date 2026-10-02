#include "CIntN.h"
#include <cstring>

CIntN::CIntN(int n)
{
    if (n < 1) n = 1;

    m_n = n;
    m_digits = new unsigned char[m_n];
    m_negative = false;

    for (int i = 0; i < m_n; i++) m_digits[i] = 0;
}

CIntN::CIntN(int n, const char *str)
{
    if (n < 1) n = 1;

    m_n = n;
    m_digits = new unsigned char[m_n];
    m_negative = false;

    for (int i = 0; i < m_n; i++) m_digits[i] = 0;

    int start = 0;
    if (str[0] == '-')
    {
        m_negative = true;
        start = 1;
    }
    else if (str[0] == '+') start = 1;

    int len = (int)strlen(str) - start;

    if (len > m_n)
    {
        m_negative = false;
        return;
    }

    for (int i = 0; i < len; i++)
    {
        char current = str[start + len - 1 - i];

        if (current < '0' || current > '9')
        {
            for (int k = 0; k < m_n; k++) m_digits[k] = 0;
            m_negative = false;
            return;
        }

        m_digits[i] = current - '0';
    }

    if (is_zero()) m_negative = false;
}

CIntN::CIntN(const CIntN &b)
{
    m_n = b.m_n;
    m_negative = b.m_negative;
    m_digits = new unsigned char[m_n];

    for (int i = 0; i < m_n; i++) m_digits[i] = b.m_digits[i];
}

CIntN::CIntN(CIntN &&b)
{
    m_n = b.m_n;
    m_digits = b.m_digits;
    m_negative = b.m_negative;

    b.m_n = 0;
    b.m_digits = nullptr;
    b.m_negative = false;
}

CIntN::~CIntN()
{
    delete []m_digits;
}

CIntN& CIntN::operator=(const CIntN &b)
{
    if (this == &b) return *this;

    delete []m_digits;

    m_n = b.m_n;
    m_negative = b.m_negative;
    m_digits = new unsigned char[m_n];

    for (int i = 0; i < m_n; i++) m_digits[i] = b.m_digits[i];

    return *this;
}

CIntN& CIntN::operator=(CIntN &&b)
{
    if (this == &b) return *this;

    delete []m_digits;

    m_n = b.m_n;
    m_digits = b.m_digits;
    m_negative = b.m_negative;

    b.m_n = 0;
    b.m_digits = nullptr;
    b.m_negative = false;

    return *this;
}

bool CIntN::is_zero() const
{
    for (int i = 0; i < m_n; i++)
        if (m_digits[i] != 0) return false;

    return true;
}

int CIntN::compare_abs(const CIntN &b) const
{
    int max_n = m_n;
    if (b.m_n > max_n) max_n = b.m_n;

    for (int i = max_n - 1; i >= 0; i--)
    {
        int first = 0;
        int second = 0;

        if (i < m_n) first = m_digits[i];
        if (i < b.m_n) second = b.m_digits[i];

        if (first > second) return 1;
        if (first < second) return -1;
    }

    return 0;
}

CIntN CIntN::operator+(const CIntN &b) const
{
    int max_n = m_n;
    if (b.m_n > max_n) max_n = b.m_n;

    if (m_negative == b.m_negative)
    {
        CIntN result(max_n + 1);
        int carry = 0;

        for (int i = 0; i < max_n; i++)
        {
            int first = 0;
            int second = 0;

            if (i < m_n) first = m_digits[i];
            if (i < b.m_n) second = b.m_digits[i];

            int sum = first + second + carry;
            result.m_digits[i] = sum % 10;
            carry = sum / 10;
        }

        result.m_digits[max_n] = carry;
        result.m_negative = m_negative;

        if (result.is_zero()) result.m_negative = false;

        return result;
    }

    int comparison = compare_abs(b);

    if (comparison == 0) return CIntN(max_n);

    const CIntN *greater;
    const CIntN *smaller;

    if (comparison > 0)
    {
        greater = this;
        smaller = &b;
    }
    else
    {
        greater = &b;
        smaller = this;
    }

    CIntN result(max_n);
    int borrow = 0;

    for (int i = 0; i < max_n; i++)
    {
        int first = 0;
        int second = 0;

        if (i < greater->m_n) first = greater->m_digits[i];
        if (i < smaller->m_n) second = smaller->m_digits[i];

        first -= borrow;

        if (first < second)
        {
            first += 10;
            borrow = 1;
        }
        else borrow = 0;

        result.m_digits[i] = first - second;
    }

    result.m_negative = greater->m_negative;

    if (result.is_zero()) result.m_negative = false;

    return result;
}

CIntN CIntN::operator-(const CIntN &b) const
{
    CIntN opposite(b);

    if (!opposite.is_zero()) opposite.m_negative = !opposite.m_negative;

    return *this + opposite;
}

std::ostream& operator<<(std::ostream &out, const CIntN &number)
{
    if (number.m_n == 0 || number.m_digits == nullptr)
    {
        out << 0;
        return out;
    }

    int first_digit = number.m_n - 1;

    while (first_digit > 0 && number.m_digits[first_digit] == 0)
        first_digit--;

    if (number.m_negative && !number.is_zero()) out << '-';

    for (int i = first_digit; i >= 0; i--)
        out << (int)number.m_digits[i];

    return out;
}
