#include "StringBuffer.hpp"

#include <iostream>

void PrintBuffer(const char* message, const StringBuffer& buffer)
{
    char* str = new char[buffer.Length() + 1];

    buffer.Copy(str);
    std::cout << message << str << std::endl;

    delete[] str;
}

int main()
{
    StringBuffer buffer(5);
    char str[100];
    char ch;

    std::cout << "Create empty buffer" << std::endl;
    std::cout << "Length: " << buffer.Length() << std::endl;

    buffer.Append("Hello");
    buffer.Append(", world!");
    PrintBuffer("After append: ", buffer);
    std::cout << "Length: " << buffer.Length() << std::endl;

    if (buffer.Insert(5, " my") == 0) {
        PrintBuffer("After insert: ", buffer);
    } else {
        std::cout << "Insert error" << std::endl;
    }

    if (buffer.GetChar(1, &ch) == 0) {
        std::cout << "Char at position 1: " << ch << std::endl;
    }

    if (buffer.SetChar(1, 'a') == 0) {
        PrintBuffer("After change char: ", buffer);
    }

    if (buffer.Copy(str) == 0) {
        std::cout << "Copied string: " << str << std::endl;
    }

    if (buffer.CopySubstring(str, 6, 2) == 0) {
        std::cout << "Copied substring: " << str << std::endl;
    } else {
        std::cout << "Copy substring error" << std::endl;
    }

    std::cout << "Position of symbol 'w': " << buffer.FindChar('w') << std::endl;
    std::cout << "Position of substring \"world\": " << buffer.FindSubstring("world") << std::endl;

    if (buffer.Replace("world", "C++") == 0) {
        PrintBuffer("After replace: ", buffer);
    } else {
        std::cout << "Replace error" << std::endl;
    }

    if (buffer.Cut(8) == 0) {
        PrintBuffer("After cut: ", buffer);
    }

    buffer.Clear();
    std::cout << "After clear length: " << buffer.Length() << std::endl;

    StringBuffer initialized("Start string", 4);
    PrintBuffer("Initialized buffer: ", initialized);

    if (initialized.Insert(100, "bad") != 0) {
        std::cout << "Incorrect insert checked" << std::endl;
    }

    if (initialized.CopySubstring(str, 3, 100) != 0) {
        std::cout << "Incorrect substring checked" << std::endl;
    }

    return 0;
}
