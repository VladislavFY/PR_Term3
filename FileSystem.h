#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

/*
Файловый контейнер. Задание 2.15.
Требуется создать виртуальный диск — большой участок памяти — для хранения
файлов из произвольных байтов. Поддерживаются операции:
- создание и удаление файла;
- копирование и переименование;
- получение списка файлов и размера файла;
- чтение/запись заданного количества байт по заданному смещению.

Вариант 2.15: файловые блоки образуют ДВУСВЯЗНЫЕ списки.
Ссылки на предыдущий и следующий блок размещаются непосредственно
в каждом блоке (здесь это номера блоков массива: prev и next).

Дополнение к условию для изучения умных указателей:
1. Виртуальный диск выделяется динамически; единственный владелец его
   массива блоков — std::unique_ptr<Block[]> внутри DiskState.
2. Добавляется операция open(name), возвращающая файловый дескриптор
   FileHandle. Несколько дескрипторов и запись в каталоге могут совместно
   владеть одним FileData с помощью std::shared_ptr.
3. После remove(name) имя сразу исчезает из каталога, но открытые
   дескрипторы могут продолжать чтение и запись. Блоки освобождаются
   только после закрытия последнего дескриптора и удаления имени.
4. Когда файловый контейнер уничтожен, открытый дескриптор остаётся
   работоспособным: FileData удерживает общее состояние виртуального диска.
5. copy(source, destination) создаёт НЕЗАВИСИМУЮ копию данных, а не
   второй дескриптор или общее имя. Операции одного FileSystem не
   выполняются параллельно из нескольких потоков.

Проверить обычные и ошибочные случаи, включая границы блоков,
недостаток памяти, удаление открытого файла и совместные дескрипторы.
*/

#include <iostream>
#include <cstring>
#include <climits>
#include <memory>
#include <cstdlib>

#define BLOCK_SIZE 64
#define MAX_FILES 100
#define NAME_SIZE 31

class FileSystem
{
private:
    struct Block
    {
        int prev;
        int next;
        bool used;
        char data[BLOCK_SIZE];
    };

    struct FileData;

    struct DiskState
    {
        std::unique_ptr<Block[]> blocks;
        int block_count;

        DiskState(int count);
        int free_blocks() const;
        int free_block();
        int block_at(const FileData &file, int position) const;
        void put_bytes(const FileData &file, int offset, const char *data, int length);
        int read_bytes(const FileData &file, int offset, char *data, int length) const;
        int write_bytes(FileData &file, int offset, const char *data, int length);
        void free_chain(FileData &file);
        bool check_links(const FileData &file) const;
    };

    struct FileData
    {
        std::shared_ptr<DiskState> state;
        int size;
        int first;
        int last;

        FileData(const std::shared_ptr<DiskState> &disk);
        ~FileData();
    };

public:
    class FileHandle
    {
    private:
        std::shared_ptr<FileData> m_file;
        FileHandle(const std::shared_ptr<FileData> &file);
        friend class FileSystem;

    public:
        FileHandle();
        bool valid() const;
        int size() const;
        int read(int offset, char *data, int length) const;
        int write(int offset, const char *data, int length);
        bool check_links() const;
    };

private:
    struct Entry
    {
        char name[NAME_SIZE + 1];
        std::shared_ptr<FileData> file;
    };

    std::shared_ptr<DiskState> m_state;
    int m_file_count;
    Entry m_files[MAX_FILES];

    int find(const char *name) const;
    int free_file() const;

public:
    FileSystem(int block_count);
    ~FileSystem();

    FileSystem(const FileSystem &b) = delete;
    FileSystem& operator=(const FileSystem &b) = delete;

    bool create(const char *name);
    bool remove(const char *name);
    bool copy(const char *source, const char *destination);
    bool rename_file(const char *old_name, const char *new_name);
    FileHandle open(const char *name) const;

    int write(const char *name, int offset, const char *data, int length);
    int read(const char *name, int offset, char *data, int length) const;

    int size(const char *name) const;
    int file_count() const;
    int free_blocks() const;
    bool check_links(const char *name) const;
    void list() const;
};

#endif
