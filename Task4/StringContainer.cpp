#include "StringContainer.h"

StringContainer::Block::Block(int block_size)
    : data(new char[block_size]), size(block_size), used(0), next(nullptr)
{
}

StringContainer::StringContainer(int block_size)
    : m_first(nullptr), m_last(nullptr), m_block_size(block_size), m_count(0)
{
    if (m_block_size <= 0) m_block_size = 64;
}

StringContainer::~StringContainer()
{
    clear();
}

char* StringContainer::add(const char *str)
{
    if (str == nullptr) return nullptr;

    size_t length = std::strlen(str) + 1;
    if (length > (size_t)INT_MAX) return nullptr;

    if (m_last == nullptr || m_last->size - m_last->used < (int)length)
    {
        int size = m_block_size;
        if (length > (size_t)size) size = (int)length;

        std::unique_ptr<Block> block(new Block(size));
        Block *place = block.get();

        if (m_last == nullptr)
            m_first = std::move(block);
        else
            m_last->next = std::move(block);

        m_last = place;
    }

    char *place = m_last->data.get() + m_last->used;
    std::memcpy(place, str, length);
    m_last->used += (int)length;
    m_count++;

    return place;
}

void StringContainer::clear()
{
    while (m_first != nullptr)
        m_first = std::move(m_first->next);

    m_last = nullptr;
    m_count = 0;
}

int StringContainer::count() const
{
    return m_count;
}
