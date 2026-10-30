#ifndef STRING_CONTAINER_H
#define STRING_CONTAINER_H

/*
Задание 1.15. Контейнер строк.
Требуется построить контейнер для размещения текстовых строк без операции удаления.
Для хранения выделяется блок памяти, в котором последовательно размещаются
поступающие строки. Если очередная строка не помещается в остатке блока,
то выделяется следующий блок. Все блоки связываются в список для осуществления
операции очистки контейнера.

Реализация должна поддерживать:
- создание пустого контейнера;
- очистку всего контейнера;
- добавление строки в контейнер (на выходе — указатель на место строки);
- получение количества хранящихся строк.
Интерфейс разрабатывается студентом. Тесты проверяют корректные и
некорректные ситуации.

Дополнение к реализации: применить std::unique_ptr для единоличного
владения блоками и массивами символов. std::shared_ptr не требуется,
поскольку всеми блоками владеет один контейнер.
*/

#include <memory>
#include <cstring>
#include <utility>
#include <iostream>
#include <cstdlib>
#include <climits>

class StringContainer
{
private:
    struct Block
    {
        std::unique_ptr<char[]> data;
        int size;
        int used;
        std::unique_ptr<Block> next;

        Block(int block_size);
    };

    std::unique_ptr<Block> m_first;
    Block *m_last;
    int m_block_size;
    int m_count;

public:
    StringContainer(int block_size = 64);
    ~StringContainer();

    StringContainer(const StringContainer &b) = delete;
    StringContainer& operator=(const StringContainer &b) = delete;

    char* add(const char *str);
    void clear();
    int count() const;
};

#endif
