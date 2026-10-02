#include "CIntN.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <string>
#include <utility>

void make_random_number(char *str, int n)
{
    int position = 0;

    if (rand() % 2 == 1) str[position++] = '-';

    if (rand() % 20 == 0)
    {
        str[position++] = '0';
        str[position] = '\0';
        return;
    }

    str[position++] = rand() % 9 + '1';

    for (int i = 1; i < n; i++)
        str[position++] = rand() % 10 + '0';

    str[position] = '\0';
}

const char* get_digits(const char *str)
{
    if (str[0] == '-' || str[0] == '+') return str + 1;
    return str;
}

bool is_negative(const char *str)
{
    return str[0] == '-';
}

int compare_unsigned_strings(const char *a, const char *b)
{
    while (*a == '0' && *(a + 1) != '\0') a++;
    while (*b == '0' && *(b + 1) != '\0') b++;

    int len_a = strlen(a);
    int len_b = strlen(b);

    if (len_a > len_b) return 1;
    if (len_a < len_b) return -1;

    int comparison = strcmp(a, b);

    if (comparison > 0) return 1;
    if (comparison < 0) return -1;
    return 0;
}

void add_unsigned_strings(const char *a, const char *b, char *result)
{
    int len_a = strlen(a);
    int len_b = strlen(b);
    int max_len = len_a;

    if (len_b > max_len) max_len = len_b;

    char *reversed = new char[max_len + 2];
    int carry = 0;
    int position = 0;

    for (int i = 0; i < max_len; i++)
    {
        int first = 0;
        int second = 0;

        if (len_a - 1 - i >= 0) first = a[len_a - 1 - i] - '0';
        if (len_b - 1 - i >= 0) second = b[len_b - 1 - i] - '0';

        int sum = first + second + carry;
        reversed[position++] = sum % 10 + '0';
        carry = sum / 10;
    }

    if (carry != 0) reversed[position++] = carry + '0';

    for (int i = 0; i < position; i++)
        result[i] = reversed[position - 1 - i];

    result[position] = '\0';

    delete []reversed;
}

void subtract_unsigned_strings(const char *a, const char *b, char *result)
{
    int len_a = strlen(a);
    int len_b = strlen(b);

    char *reversed = new char[len_a + 1];
    int borrow = 0;

    for (int i = 0; i < len_a; i++)
    {
        int first = a[len_a - 1 - i] - '0' - borrow;
        int second = 0;

        if (len_b - 1 - i >= 0) second = b[len_b - 1 - i] - '0';

        if (first < second)
        {
            first += 10;
            borrow = 1;
        }
        else borrow = 0;

        reversed[i] = first - second + '0';
    }

    int last = len_a - 1;
    while (last > 0 && reversed[last] == '0') last--;

    int position = 0;

    for (int i = last; i >= 0; i--)
        result[position++] = reversed[i];

    result[position] = '\0';

    delete []reversed;
}

void write_signed_result(bool negative, const char *digits, char *result)
{
    int position = 0;

    if (negative && strcmp(digits, "0") != 0)
        result[position++] = '-';

    for (int i = 0; digits[i] != '\0'; i++)
        result[position++] = digits[i];

    result[position] = '\0';
}

void add_strings(const char *a, const char *b, char *result)
{
    bool negative_a = is_negative(a);
    bool negative_b = is_negative(b);

    const char *digits_a = get_digits(a);
    const char *digits_b = get_digits(b);

    int max_len = strlen(digits_a);
    if ((int)strlen(digits_b) > max_len) max_len = strlen(digits_b);

    char *digits_result = new char[max_len + 2];

    if (negative_a == negative_b)
    {
        add_unsigned_strings(digits_a, digits_b, digits_result);
        write_signed_result(negative_a, digits_result, result);
    }
    else
    {
        int comparison = compare_unsigned_strings(digits_a, digits_b);

        if (comparison == 0)
        {
            strcpy(result, "0");
        }
        else if (comparison > 0)
        {
            subtract_unsigned_strings(digits_a, digits_b, digits_result);
            write_signed_result(negative_a, digits_result, result);
        }
        else
        {
            subtract_unsigned_strings(digits_b, digits_a, digits_result);
            write_signed_result(negative_b, digits_result, result);
        }
    }

    delete []digits_result;
}

void subtract_strings(const char *a, const char *b, char *result)
{
    int len_b = strlen(b);
    char *opposite_b = new char[len_b + 2];

    if (b[0] == '-')
    {
        strcpy(opposite_b, b + 1);
    }
    else
    {
        opposite_b[0] = '-';
        strcpy(opposite_b + 1, b);
    }

    add_strings(a, opposite_b, result);

    delete []opposite_b;
}

std::string to_string(const CIntN &number)
{
    std::ostringstream out;
    out << number;
    return out.str();
}

int main()
{
    srand((unsigned)time(nullptr));

    int tests;

    std::cout << "Enter number of tests: ";
    std::cin >> tests;

    if (tests <= 0)
    {
        std::cout << "Wrong number of tests" << std::endl;
        return 1;
    }

    int passed = 0;

    for (int test = 1; test <= tests; test++)
    {
        int n_a = rand() % 30 + 1;
        int n_b = rand() % 30 + 1;

        char *a_string = new char[n_a + 2];
        char *b_string = new char[n_b + 2];

        int max_n = n_a;
        if (n_b > max_n) max_n = n_b;

        char *expected_sum = new char[max_n + 3];
        char *expected_difference = new char[max_n + 3];

        make_random_number(a_string, n_a);
        make_random_number(b_string, n_b);

        CIntN a(n_a, a_string);
        CIntN b(n_b, b_string);

        CIntN sum = a + b;
        CIntN difference = a - b;

        add_strings(a_string, b_string, expected_sum);
        subtract_strings(a_string, b_string, expected_difference);

        CIntN copy(sum);
        CIntN assigned(1);
        assigned = difference;

        CIntN moved(std::move(copy));
        CIntN move_assigned(1);
        move_assigned = std::move(assigned);

        bool correct =
            to_string(sum) == expected_sum &&
            to_string(difference) == expected_difference &&
            to_string(moved) == expected_sum &&
            to_string(move_assigned) == expected_difference;

        if (!correct)
        {
            std::cout << "Test " << test << " failed" << std::endl;
            std::cout << "a = " << a << std::endl;
            std::cout << "b = " << b << std::endl;
            std::cout << "a + b = " << sum << std::endl;
            std::cout << "a - b = " << difference << std::endl;
            std::cout << "expected sum = " << expected_sum << std::endl;
            std::cout << "expected difference = " << expected_difference << std::endl;

            delete []a_string;
            delete []b_string;
            delete []expected_sum;
            delete []expected_difference;

            return 1;
        }

        passed++;

        std::cout << "Test " << test << " passed for operators + and -" << std::endl;
        std::cout << "a = " << a << std::endl;
        std::cout << "b = " << b << std::endl;
        std::cout << "a + b = " << sum << std::endl;
        std::cout << "a - b = " << difference << std::endl;
        std::cout << "expected sum = " << expected_sum << std::endl;
        std::cout << "expected difference = " << expected_difference << std::endl;
        std::cout << std::endl;

        delete []a_string;
        delete []b_string;
        delete []expected_sum;
        delete []expected_difference;
    }

    std::cout << "Passed: " << passed << " / " << tests << std::endl;
    std::cout << "All tests passed. Class CIntN works correctly." << std::endl;

    return 0;
}
