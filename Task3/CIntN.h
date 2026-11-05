#ifndef CINTN_H
#define CINTN_H

#include <memory>
#include <ostream>

class CIntN
{
private:
    int m_n;
    std::unique_ptr<unsigned char[]> m_digits;
    bool m_negative;

    bool is_zero() const;
    int compare_abs(const CIntN &b) const;

public:
    // Constructors and destructor:
    CIntN(int n = 1);
    CIntN(int n, const char *str);
    CIntN(const CIntN &b); // copy constructor
    // Move constructor works as O(1) instead of copy constructor O(N).
    CIntN(CIntN &&b) noexcept; // move constructor
    ~CIntN() = default;

    // Assignments:
    CIntN& operator=(const CIntN &b); // copy assignment
    CIntN& operator=(CIntN &&b) noexcept; // move assignment

    // Operators:
    CIntN operator+(const CIntN &b) const;
    CIntN operator-(const CIntN &b) const;
    CIntN operator*(const CIntN &b) const;

    bool operator==(const CIntN &b) const;

    friend std::ostream& operator<<(std::ostream &out, const CIntN &number);
};

#endif
