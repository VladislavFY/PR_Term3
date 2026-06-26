#pragma once

#include <cstddef>

class StringBuffer {
public:
    StringBuffer(size_t piece_size = 16);
    StringBuffer(const char* str, size_t piece_size = 16);
    StringBuffer(const StringBuffer& other);
    StringBuffer& operator=(const StringBuffer& other);
    ~StringBuffer();

    void Clear();
    int Append(const char* str);
    int Insert(size_t pos, const char* str);
    size_t Length() const;

    int Copy(char* dest) const;
    int CopySubstring(char* dest, size_t pos, size_t count) const;

    int GetChar(size_t pos, char* ch) const;
    int SetChar(size_t pos, char ch);

    int FindChar(char ch) const;
    int FindSubstring(const char* str) const;

    int Replace(const char* old_str, const char* new_str);
    int Cut(size_t new_length);

private:
    struct Piece {
        char* data;
        size_t used;
        Piece* next;
    };

    Piece* m_head;
    Piece* m_tail;
    size_t m_piece_size;
    size_t m_length;

    Piece* CreatePiece() const;
    int PushChar(char ch);
    char GetCharByIndex(size_t pos) const;
    char* ToCString() const;
    int BuildFromCString(const char* str);
};
