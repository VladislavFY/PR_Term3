#include "StringContainer.h"


int passed = 0;
int total = 0;

void check(bool correct, const char *message)
{
    total++;

    if (correct)
        passed++;
    else
        std::cout << "FAILED: " << message << std::endl;
}

int main()
{
    StringContainer a(12);

    check(a.count() == 0, "empty container");
    check(a.add(nullptr) == nullptr, "null string");
    check(a.count() == 0, "count after null string");

    char *first = a.add("abc");
    char *second = a.add("12");
    char *third = a.add("");

    check(first != nullptr && std::strcmp(first, "abc") == 0, "first string");
    check(second == first + 4, "strings stored consecutively");
    check(third == second + 3, "empty string stored consecutively");
    check(a.count() == 3, "count after three strings");

    char *large = a.add("This text is longer than one usual block");
    check(large != nullptr && std::strcmp(large, "This text is longer than one usual block") == 0,
          "string larger than block");
    check(std::strcmp(first, "abc") == 0, "old pointer after new block");

    a.clear();
    check(a.count() == 0, "clear");
    a.clear();
    check(a.count() == 0, "second clear");
    check(std::strcmp(a.add("again"), "again") == 0, "reuse after clear");

    std::cout << "Enter number of random tests: ";
    int n;
    if (!(std::cin >> n) || n < 0 || n > 100000)
    {
        std::cout << "Incorrect number of tests" << std::endl;
        return 1;
    }

    StringContainer b(32);
    char **addresses = new char*[n];
    char (*values)[101] = new char[n][101];

    std::srand(12345);

    for (int i = 0; i < n; i++)
    {
        int length = std::rand() % 101;

        for (int j = 0; j < length; j++)
            values[i][j] = 'a' + std::rand() % 26;

        values[i][length] = '\0';
        addresses[i] = b.add(values[i]);
    }

    for (int i = 0; i < n; i++)
    {
        check(addresses[i] != nullptr && std::strcmp(addresses[i], values[i]) == 0,
              "random string or pointer changed");
    }

    check(b.count() == n, "random strings count");
    b.clear();
    check(b.count() == 0, "clear after random tests");

    delete []addresses;
    delete []values;

    // Many very small blocks: clear() must not recursively delete the chain.
    StringContainer long_list(1);
    bool long_ok = true;
    for (int i = 0; i < 10000; i++)
        if (long_list.add("x") == nullptr) long_ok = false;
    check(long_ok && long_list.count() == 10000, "many blocks");
    long_list.clear();
    check(long_list.count() == 0, "iterative clear of many blocks");

    std::cout << "Passed: " << passed << " / " << total << std::endl;
    if (passed == total)
        std::cout << "All tests passed." << std::endl;

    return passed == total ? 0 : 1;
}
