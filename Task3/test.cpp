#include "CIntN.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <memory>
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

    std::unique_ptr<char[]> reversed = std::make_unique<char[]>(max_len + 2);
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
}

void subtract_unsigned_strings(const char *a, const char *b, char *result)
{
    int len_a = strlen(a);
    int len_b = strlen(b);

    std::unique_ptr<char[]> reversed = std::make_unique<char[]>(len_a + 1);
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
}

void write_signed_result(bool negative, const char *digits, char *result)
{
    int start = 0;
    while (digits[start] == '0' && digits[start + 1] != '\0') start++;

    int position = 0;
    if (negative && digits[start] != '0') result[position++] = '-';

    for (int i = start; digits[i] != '\0'; i++)
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

    std::unique_ptr<char[]> digits_result = std::make_unique<char[]>(max_len + 2);

    if (negative_a == negative_b)
    {
        add_unsigned_strings(digits_a, digits_b, digits_result.get());
        write_signed_result(negative_a, digits_result.get(), result);
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
            subtract_unsigned_strings(digits_a, digits_b, digits_result.get());
            write_signed_result(negative_a, digits_result.get(), result);
        }
        else
        {
            subtract_unsigned_strings(digits_b, digits_a, digits_result.get());
            write_signed_result(negative_b, digits_result.get(), result);
        }
    }
}

void subtract_strings(const char *a, const char *b, char *result)
{
    int len_b = strlen(b);
    std::unique_ptr<char[]> opposite_b = std::make_unique<char[]>(len_b + 2);

    if (b[0] == '-')
    {
        strcpy(opposite_b.get(), b + 1);
    }
    else
    {
        opposite_b[0] = '-';
        strcpy(opposite_b.get() + 1, b);
    }

    add_strings(a, opposite_b.get(), result);
}

// Independent schoolbook multiplication, used only in the test.
void multiply_strings(const char *a, const char *b, char *result)
{
    bool negative = (is_negative(a) != is_negative(b));
    const char *first = get_digits(a);
    const char *second = get_digits(b);
    int n = std::strlen(first);
    int m = std::strlen(second);
    int length = n + m;

    std::unique_ptr<int[]> digits = std::make_unique<int[]>(length);
    for (int i = 0; i < length; i++) digits[i] = 0;

    for (int i = n - 1; i >= 0; i--)
    {
        int carry = 0;
        for (int j = m - 1; j >= 0; j--)
        {
            int position = i + j + 1;
            int sum = (first[i] - '0') * (second[j] - '0')
                      + digits[position] + carry;
            digits[position] = sum % 10;
            carry = sum / 10;
        }
        digits[i] += carry;
    }

    int start = 0;
    while (start < length - 1 && digits[start] == 0) start++;

    int position = 0;
    if (negative && !(start == length - 1 && digits[start] == 0))
        result[position++] = '-';

    for (int i = start; i < length; i++)
        result[position++] = digits[i] + '0';

    result[position] = '\0';
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

bool valid_number(const char *text, int n)
{
    int start = 0;
    if (text[0] == '-' || text[0] == '+') start = 1;

    int len = std::strlen(text) - start;
    if (len < 1 || len > n) return false;

    for (int i = start; text[i] != '\0'; i++)
        if (text[i] < '0' || text[i] > '9') return false;

    return true;
}

bool check_result(int n_a, const char *a_string, int n_b,
                  const char *b_string, bool show)
{
    int max_n = n_a;
    if (n_b > max_n) max_n = n_b;

    std::unique_ptr<char[]> expected_sum = std::make_unique<char[]>(max_n + 3);
    std::unique_ptr<char[]> expected_difference = std::make_unique<char[]>(max_n + 3);
    std::unique_ptr<char[]> expected_product = std::make_unique<char[]>(n_a + n_b + 3);

    add_strings(a_string, b_string, expected_sum.get());
    subtract_strings(a_string, b_string, expected_difference.get());
    multiply_strings(a_string, b_string, expected_product.get());

    CIntN a(n_a, a_string);
    CIntN b(n_b, b_string);
    CIntN sum = a + b;
    CIntN difference = a - b;
    CIntN product = a * b;

    CIntN expected_sum_number(max_n + 1, expected_sum.get());
    CIntN expected_difference_number(max_n + 1, expected_difference.get());
    CIntN expected_product_number(n_a + n_b, expected_product.get());

    CIntN copy(product);
    CIntN assigned(1);
    assigned = difference;

    CIntN moved(std::move(copy));
    CIntN move_assigned(1);
    move_assigned = std::move(assigned);

    bool correct = sum == expected_sum_number &&
                   difference == expected_difference_number &&
                   product == expected_product_number &&
                   moved == expected_product_number &&
                   move_assigned == expected_difference_number;

    if (show || !correct)
    {
        std::cout << "a = " << a << "\n";
        std::cout << "b = " << b << "\n";
        std::cout << "a + b = " << sum << "\n";
        std::cout << "a - b = " << difference << "\n";
        std::cout << "a * b = " << product << "\n";
        std::cout << "expected sum = " << expected_sum.get() << "\n";
        std::cout << "expected difference = " << expected_difference.get() << "\n";
        std::cout << "expected product = " << expected_product.get() << "\n";
        std::cout << (correct ? "PASSED" : "FAILED") << "\n";
    }
    return correct;
}

bool smart_pointer_test()
{
    std::unique_ptr<CIntN> first = std::make_unique<CIntN>(3, "123");
    std::unique_ptr<CIntN> moved = std::move(first);

    std::shared_ptr<CIntN> shared = std::make_shared<CIntN>(3, "123");
    std::shared_ptr<CIntN> shared_copy = shared;

    return first == nullptr && *moved == *shared &&
           shared == shared_copy && shared.use_count() == 2;
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

    if (!check_result(3, "123", 2, "67", false) ||
        !check_result(4, "9999", 4, "9999", false) ||
        !check_result(3, "-123", 2, "67", false) ||
        !check_result(3, "-123", 2, "-67", false) ||
        !check_result(3, "-0", 4, "0000", false) ||
        !check_result(4, "0007", 3, "-03", false))
    {
        std::cout << "Fixed boundary tests FAILED.\n";
        return;
    }
    std::cout << "Fixed boundary tests PASSED.\n";

    if (!smart_pointer_test())
    {
        std::cout << "Smart pointer tests FAILED.\n";
        return;
    }
    std::cout << "Smart pointer tests PASSED.\n";

    // Reproducible random tests.
    std::srand(1);
    int passed = 0;

    for (int test = 1; test <= tests; test++)
    {
        int n_a = rand() % 30 + 1;
        int n_b = rand() % 30 + 1;
        std::unique_ptr<char[]> a_string = std::make_unique<char[]>(n_a + 2);
        std::unique_ptr<char[]> b_string = std::make_unique<char[]>(n_b + 2);

        make_random_number(a_string.get(), n_a);
        make_random_number(b_string.get(), n_b);

        std::cout << "Test " << test << "\n";
        bool correct = check_result(n_a, a_string.get(), n_b, b_string.get(), true);

        if (!correct) break;
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
    char text[40];
    std::cout << "Enter N for first number: ";
    int status = read_line(text, 40);
    if (status == 0) return;
    int n_a = status == 1 ? read_positive(text, 10000) : -1;
    if (n_a < 1)
    {
        std::cout << "Error: N must be between 1 and 10000.\n";
        return;
    }

    std::unique_ptr<char[]> a = std::make_unique<char[]>(n_a + 3);
    std::cout << "Enter first number: ";
    status = read_line(a.get(), n_a + 3);
    if (status == 0) return;
    if (status != 1 || !valid_number(a.get(), n_a))
    {
        std::cout << "Error: enter a signed number with 1 to " << n_a << " digits.\n";
        return;
    }

    std::cout << "Enter N for second number: ";
    status = read_line(text, 40);
    if (status == 0) return;
    int n_b = status == 1 ? read_positive(text, 10000) : -1;
    if (n_b < 1)
    {
        std::cout << "Error: N must be between 1 and 10000.\n";
        return;
    }

    std::unique_ptr<char[]> b = std::make_unique<char[]>(n_b + 3);
    std::cout << "Enter second number: ";
    status = read_line(b.get(), n_b + 3);
    if (status == 0) return;
    if (status != 1 || !valid_number(b.get(), n_b))
    {
        std::cout << "Error: enter a signed number with 1 to " << n_b << " digits.\n";
        return;
    }

    check_result(n_a, a.get(), n_b, b.get(), true);
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
