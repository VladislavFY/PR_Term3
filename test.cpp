#include "cintn.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

void make_random_number(char *number)
{
    for (int i = 0; i < N; i++)
        number[i] = '0' + rand() % 10;

    number[N] = '\0';
}

void add_strings(const char *a, const char *b, char *result)
{
    int carry = 0;
    result[N] = '\0';

    for (int i = N - 1; i >= 0; i--)
    {
        int sum = (a[i] - '0') + (b[i] - '0') + carry;
        result[i] = '0' + sum % 10;
        carry = sum / 10;
    }
}

void subtract_strings(const char *a, const char *b, char *result)
{
    int borrow = 0;
    result[N] = '\0';

    for (int i = N - 1; i >= 0; i--)
    {
        int difference = (a[i] - '0') - (b[i] - '0') - borrow;

        if (difference < 0)
        {
            difference += 10;
            borrow = 1;
        }
        else
            borrow = 0;

        result[i] = '0' + difference;
    }
}

int run_test(int test_number)
{
    char a_str[N + 1];
    char b_str[N + 1];
    char sum_str[N + 1];
    char difference_str[N + 1];

    make_random_number(a_str);
    make_random_number(b_str);

    add_strings(a_str, b_str, sum_str);
    subtract_strings(a_str, b_str, difference_str);

    CIntN a(a_str);
    CIntN b(b_str);
    CIntN expected_sum(sum_str);
    CIntN expected_difference(difference_str);

    CIntN real_sum = a + b;
    CIntN real_difference = a - b;

    if (!(real_sum == expected_sum))
    {
        std::cout << "Test " << test_number << " failed for operator +" << std::endl;
        std::cout << "a = " << a << std::endl;
        std::cout << "b = " << b << std::endl;
        std::cout << "a + b = " << real_sum << std::endl;
        std::cout << "expected = " << expected_sum << std::endl;
        return 0;
    }

    if (!(real_difference == expected_difference))
    {
        std::cout << "Test " << test_number << " failed for operator -" << std::endl;
        std::cout << "a = " << a << std::endl;
        std::cout << "b = " << b << std::endl;
        std::cout << "a - b = " << real_difference << std::endl;
        std::cout << "expected = " << expected_difference << std::endl;
        return 0;
    }

    return 1;
}

int main()
{
    int test_count;
    int passed = 0;

    std::cout << "N = " << N << std::endl;
    std::cout << "Enter number of tests: ";

    if (!(std::cin >> test_count) || test_count <= 0)
    {
        std::cout << "Invalid number of tests." << std::endl;
        return 1;
    }

    srand((unsigned int)time(NULL));

    for (int i = 1; i <= test_count; i++)
    {
        if (run_test(i))
            passed++;
        else
            break;
    }

    std::cout << "Passed: " << passed << " / " << test_count << std::endl;

    if (passed == test_count)
    {
        std::cout << "All tests passed. Class CIntN works correctly." << std::endl;
        return 0;
    }

    std::cout << "There is an error in class CIntN." << std::endl;
    return 1;
}
