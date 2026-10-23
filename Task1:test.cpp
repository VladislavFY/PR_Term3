#include "CIntN.h"

void make_random_number(char *number)
{
    for (int i = 0; i < N; i++)
        number[i] = rand() % 10 + '0';
    number[N] = '\0';
}

void add_strings(const char *a, const char *b, char *result)
{
    int carry = 0;
    result[N] = '\0';

    for (int i = N - 1; i >= 0; i--)
    {
        int sum = a[i] - '0' + b[i] - '0' + carry;
        result[i] = sum % 10 + '0';
        carry = sum / 10;
    }
}

void subtract_strings(const char *a, const char *b, char *result)
{
    int borrow = 0;
    result[N] = '\0';

    for (int i = N - 1; i >= 0; i--)
    {
        int difference = a[i] - '0' - (b[i] - '0') - borrow;
        if (difference < 0)
        {
            difference += 10;
            borrow = 1;
        }
        else borrow = 0;

        result[i] = difference + '0';
    }
}

// 1: success, 0: end of input, -1: line is too long.
int read_line(char *text, int size)
{
    std::cin.getline(text, size);
    if (std::cin) return 1;
    if (std::cin.eof()) return 0;

    std::cin.clear();
    char c;
    while (std::cin.get(c) && c != '\n') {}
    return -1;
}

int read_positive(const char *text, int maximum)
{
    if (text[0] == '\0') return -1;
    int number = 0;

    for (int i = 0; text[i] != '\0'; i++)
    {
        if (text[i] < '0' || text[i] > '9') return -1;
        int digit = text[i] - '0';
        if (number > (maximum - digit) / 10) return -1;
        number = number * 10 + digit;
    }

    if (number == 0) return -1;
    return number;
}

bool valid_number(const char *text)
{
    int len = std::strlen(text);
    if (len == 0 || len > N) return false;

    for (int i = 0; i < len; i++)
        if (text[i] < '0' || text[i] > '9') return false;

    return true;
}

void fill_number(const char *text, char *result)
{
    int len = std::strlen(text);
    for (int i = 0; i < N - len; i++) result[i] = '0';
    for (int i = 0; i < len; i++) result[N - len + i] = text[i];
    result[N] = '\0';
}

bool check_result(const char *a_string, const char *b_string, bool show)
{
    char sum_string[N + 1];
    char difference_string[N + 1];

    add_strings(a_string, b_string, sum_string);
    subtract_strings(a_string, b_string, difference_string);

    CIntN a(a_string);
    CIntN b(b_string);
    CIntN sum = a + b;
    CIntN difference = a - b;
    CIntN expected_sum(sum_string);
    CIntN expected_difference(difference_string);

    bool correct = sum == expected_sum && difference == expected_difference;

    if (show || !correct)
    {
        std::cout << "a = " << a << "\n";
        std::cout << "b = " << b << "\n";
        std::cout << "a + b = " << sum << "\n";
        std::cout << "a - b = " << difference << "\n";
        std::cout << "expected sum = " << expected_sum << "\n";
        std::cout << "expected difference = " << expected_difference << "\n";
        std::cout << (correct ? "PASSED" : "FAILED") << "\n";
    }
    return correct;
}

void automatic_test()
{
    char text[40];
    std::cout << "Enter number of tests: ";
    int status = read_line(text, 40);
    if (status == 0) return;

    int tests = status == 1 ? read_positive(text, 1000000) : -1;
    if (tests < 1)
    {
        std::cout << "Error: invalid number of tests.\n";
        return;
    }

    char zero[N + 1];
    char one[N + 1];
    char maximum[N + 1];
    for (int i = 0; i < N; i++)
    {
        zero[i] = '0';
        one[i] = '0';
        maximum[i] = '9';
    }
    zero[N] = one[N] = maximum[N] = '\0';
    one[N - 1] = '1';

    if (!check_result(maximum, one, false) ||
        !check_result(zero, one, false) ||
        !check_result(maximum, maximum, false))
    {
        std::cout << "Fixed boundary tests FAILED.\n";
        return;
    }
    std::cout << "Fixed boundary tests PASSED.\n";

    int passed = 0;

    for (int test = 1; test <= tests; test++)
    {
        char a[N + 1];
        char b[N + 1];
        make_random_number(a);
        make_random_number(b);

        std::cout << "Test " << test << "\n";
        if (!check_result(a, b, true)) break;
        passed++;
        std::cout << "\n";
    }

    std::cout << "Passed: " << passed << " / " << tests << "\n";
    if (passed == tests)
        std::cout << "All tests passed. Class CIntN works correctly.\n";
    else
        std::cout << "There is an error in class CIntN.\n";
}

void manual_test()
{
    char first[N + 3];
    char second[N + 3];
    char a[N + 1];
    char b[N + 1];

    std::cout << "N = " << N << "\n";
    std::cout << "Enter first number: ";
    int status = read_line(first, N + 3);
    if (status == 0) return;
    if (status != 1 || !valid_number(first))
    {
        std::cout << "Error: enter an unsigned number with 1 to " << N << " digits.\n";
        return;
    }

    std::cout << "Enter second number: ";
    status = read_line(second, N + 3);
    if (status == 0) return;
    if (status != 1 || !valid_number(second))
    {
        std::cout << "Error: enter an unsigned number with 1 to " << N << " digits.\n";
        return;
    }

    fill_number(first, a);
    fill_number(second, b);
    check_result(a, b, true);
}

int main()
{
    char option[32];

    while (true)
    {
        std::cout << "\nChoose a test option:\n";
        std::cout << "0 - Exit\n";
        std::cout << "1 - Automatic test\n";
        std::cout << "2 - Manual test\n";
        std::cout << "Your choice: ";

        int status = read_line(option, 32);
        if (status == 0) break;
        if (status != 1)
        {
            std::cout << "Error: invalid test option.\n";
            continue;
        }

        if (std::strcmp(option, "0") == 0) break;
        else if (std::strcmp(option, "1") == 0) automatic_test();
        else if (std::strcmp(option, "2") == 0) manual_test();
        else std::cout << "Error: invalid test option.\n";
    }

    return 0;
}
