#ifndef CINTN_H
#define CINTN_H

#include <iostream>
#include <cstring> //strlen used
#include <cstdlib> //psuedorandom numbers
#include <utility> //std::move

class CIntN
{
private:
    int m_n; //dynamic amount of numbers
    unsigned char *m_digits;
    bool m_negative;

    bool is_zero() const;
    int compare_abs(const CIntN &b) const;

public:
	//Constructors and destructor:
    CIntN(int n = 1);
    CIntN(int n, const char *str);
    CIntN(const CIntN &b); // copy constructor
    //move constructor works as O(1) instead of copy constructor O(N)
    CIntN(CIntN &&b); //move constructor
    ~CIntN();

	//Assignments:
    CIntN& operator=(const CIntN &b); //copy assignemnt O(N)
    CIntN& operator=(CIntN &&b); //move assignment O(1)
	
	//Operators:
    CIntN operator+(const CIntN &b) const;
    CIntN operator-(const CIntN &b) const;
    CIntN operator*(const CIntN &b) const;
    bool operator==(const CIntN &b) const;
    friend std::ostream& operator<<(std::ostream &out, const CIntN &number);
};

#endif
