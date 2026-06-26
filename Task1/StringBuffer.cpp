#include "StringBuffer.hpp"

#include <cstring>

StringBuffer::StringBuffer(size_t piece_size)
{
    m_head = nullptr;
    m_tail = nullptr;
    m_length = 0;

    if (piece_size == 0) {
        m_piece_size = 16;
    } else {
        m_piece_size = piece_size;
    }
}

StringBuffer::StringBuffer(const char* str, size_t piece_size)
{
    m_head = nullptr;
    m_tail = nullptr;
    m_length = 0;

    if (piece_size == 0) {
        m_piece_size = 16;
    } else {
        m_piece_size = piece_size;
    }

    Append(str);
}

StringBuffer::StringBuffer(const StringBuffer& other)
{
    m_head = nullptr;
    m_tail = nullptr;
    m_length = 0;
    m_piece_size = other.m_piece_size;

    char* str = other.ToCString();
    Append(str);
    delete[] str;
}

StringBuffer& StringBuffer::operator=(const StringBuffer& other)
{
    if (this == &other) {
        return *this;
    }

    Clear();
    m_piece_size = other.m_piece_size;

    char* str = other.ToCString();
    Append(str);
    delete[] str;

    return *this;
}

StringBuffer::~StringBuffer()
{
    Clear();
}

StringBuffer::Piece* StringBuffer::CreatePiece() const
{
    Piece* piece = new Piece;
    piece->data = new char[m_piece_size];
    piece->used = 0;
    piece->next = nullptr;

    return piece;
}

void StringBuffer::Clear()
{
    Piece* current = m_head;

    while (current != nullptr) {
        Piece* next = current->next;
        delete[] current->data;
        delete current;
        current = next;
    }

    m_head = nullptr;
    m_tail = nullptr;
    m_length = 0;
}

int StringBuffer::PushChar(char ch)
{
    if (m_tail == nullptr || m_tail->used == m_piece_size) {
        Piece* piece = CreatePiece();

        if (m_head == nullptr) {
            m_head = piece;
        } else {
            m_tail->next = piece;
        }

        m_tail = piece;
    }

    m_tail->data[m_tail->used] = ch;
    m_tail->used++;
    m_length++;

    return 0;
}

int StringBuffer::Append(const char* str)
{
    if (str == nullptr) {
        return -1;
    }

    for (size_t i = 0; str[i] != '\0'; ++i) {
        PushChar(str[i]);
    }

    return 0;
}

int StringBuffer::Insert(size_t pos, const char* str)
{
    if (str == nullptr || pos > m_length) {
        return -1;
    }

    char* old_buffer = ToCString();
    size_t add_length = std::strlen(str);
    char* new_buffer = new char[m_length + add_length + 1];

    std::memcpy(new_buffer, old_buffer, pos);
    std::memcpy(new_buffer + pos, str, add_length);
    std::strcpy(new_buffer + pos + add_length, old_buffer + pos);

    Clear();
    Append(new_buffer);

    delete[] old_buffer;
    delete[] new_buffer;

    return 0;
}

size_t StringBuffer::Length() const
{
    return m_length;
}

int StringBuffer::Copy(char* dest) const
{
    if (dest == nullptr) {
        return -1;
    }

    size_t index = 0;
    Piece* current = m_head;

    while (current != nullptr) {
        for (size_t i = 0; i < current->used; ++i) {
            dest[index] = current->data[i];
            index++;
        }

        current = current->next;
    }

    dest[index] = '\0';

    return 0;
}

int StringBuffer::CopySubstring(char* dest, size_t pos, size_t count) const
{
    if (dest == nullptr || pos > m_length || pos + count > m_length) {
        return -1;
    }

    for (size_t i = 0; i < count; ++i) {
        dest[i] = GetCharByIndex(pos + i);
    }

    dest[count] = '\0';

    return 0;
}

char StringBuffer::GetCharByIndex(size_t pos) const
{
    Piece* current = m_head;
    size_t skipped = 0;

    while (current != nullptr) {
        if (pos < skipped + current->used) {
            return current->data[pos - skipped];
        }

        skipped += current->used;
        current = current->next;
    }

    return '\0';
}

int StringBuffer::GetChar(size_t pos, char* ch) const
{
    if (ch == nullptr || pos >= m_length) {
        return -1;
    }

    *ch = GetCharByIndex(pos);

    return 0;
}

int StringBuffer::SetChar(size_t pos, char ch)
{
    if (pos >= m_length) {
        return -1;
    }

    Piece* current = m_head;
    size_t skipped = 0;

    while (current != nullptr) {
        if (pos < skipped + current->used) {
            current->data[pos - skipped] = ch;
            return 0;
        }

        skipped += current->used;
        current = current->next;
    }

    return -1;
}

int StringBuffer::FindChar(char ch) const
{
    size_t index = 0;
    Piece* current = m_head;

    while (current != nullptr) {
        for (size_t i = 0; i < current->used; ++i) {
            if (current->data[i] == ch) {
                return static_cast<int>(index);
            }

            index++;
        }

        current = current->next;
    }

    return -1;
}

int StringBuffer::FindSubstring(const char* str) const
{
    if (str == nullptr || str[0] == '\0') {
        return -1;
    }

    char* buffer = ToCString();
    char* found = std::strstr(buffer, str);
    int result = -1;

    if (found != nullptr) {
        result = static_cast<int>(found - buffer);
    }

    delete[] buffer;

    return result;
}

int StringBuffer::Replace(const char* old_str, const char* new_str)
{
    if (old_str == nullptr || new_str == nullptr || old_str[0] == '\0') {
        return -1;
    }

    char* buffer = ToCString();
    char* found = std::strstr(buffer, old_str);

    if (found == nullptr) {
        delete[] buffer;
        return -1;
    }

    size_t old_length = std::strlen(old_str);
    size_t new_length = std::strlen(new_str);
    size_t prefix_length = static_cast<size_t>(found - buffer);
    size_t suffix_length = std::strlen(found + old_length);
    char* result = new char[prefix_length + new_length + suffix_length + 1];

    std::memcpy(result, buffer, prefix_length);
    std::memcpy(result + prefix_length, new_str, new_length);
    std::strcpy(result + prefix_length + new_length, found + old_length);

    Clear();
    Append(result);

    delete[] buffer;
    delete[] result;

    return 0;
}

int StringBuffer::Cut(size_t new_length)
{
    if (new_length > m_length) {
        return -1;
    }

    if (new_length == m_length) {
        return 0;
    }

    char* buffer = ToCString();
    buffer[new_length] = '\0';

    Clear();
    Append(buffer);

    delete[] buffer;

    return 0;
}

char* StringBuffer::ToCString() const
{
    char* buffer = new char[m_length + 1];
    Copy(buffer);

    return buffer;
}

int StringBuffer::BuildFromCString(const char* str)
{
    Clear();
    return Append(str);
}
