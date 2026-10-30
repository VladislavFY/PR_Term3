#include "FileSystem.h"

FileSystem::DiskState::DiskState(int count)
    : blocks(nullptr), block_count(count)
{
    if (block_count < 0) block_count = 0;
    blocks.reset(new Block[block_count]);

    for (int i = 0; i < block_count; i++)
    {
        blocks[i].used = false;
        blocks[i].prev = -1;
        blocks[i].next = -1;
    }
}

int FileSystem::DiskState::free_blocks() const
{
    int count = 0;
    for (int i = 0; i < block_count; i++)
        if (!blocks[i].used) count++;
    return count;
}

int FileSystem::DiskState::free_block()
{
    for (int i = 0; i < block_count; i++)
    {
        if (!blocks[i].used)
        {
            blocks[i].used = true;
            blocks[i].prev = -1;
            blocks[i].next = -1;
            std::memset(blocks[i].data, 0, BLOCK_SIZE);
            return i;
        }
    }
    return -1;
}

int FileSystem::DiskState::block_at(const FileData &file, int position) const
{
    int block = file.first;
    for (int i = 0; i < position && block != -1; i++)
        block = blocks[block].next;
    return block;
}

void FileSystem::DiskState::put_bytes(const FileData &file, int offset,
                                      const char *data, int length)
{
    int block = block_at(file, offset / BLOCK_SIZE);
    int inside = offset % BLOCK_SIZE;
    int done = 0;

    while (done < length)
    {
        int portion = BLOCK_SIZE - inside;
        if (portion > length - done) portion = length - done;

        if (data == nullptr)
            std::memset(blocks[block].data + inside, 0, portion);
        else
            std::memcpy(blocks[block].data + inside, data + done, portion);

        done += portion;
        inside = 0;
        block = blocks[block].next;
    }
}

int FileSystem::DiskState::read_bytes(const FileData &file, int offset,
                                      char *data, int length) const
{
    if (offset < 0 || length < 0) return -1;
    if (length == 0) return 0;
    if (data == nullptr) return -1;
    if (offset >= file.size) return 0;

    int available = file.size - offset;
    if (length > available) length = available;

    int block = block_at(file, offset / BLOCK_SIZE);
    int inside = offset % BLOCK_SIZE;
    int done = 0;

    while (done < length)
    {
        int portion = BLOCK_SIZE - inside;
        if (portion > length - done) portion = length - done;
        std::memcpy(data + done, blocks[block].data + inside, portion);
        done += portion;
        inside = 0;
        block = blocks[block].next;
    }
    return length;
}

int FileSystem::DiskState::write_bytes(FileData &file, int offset,
                                       const char *data, int length)
{
    if (offset < 0 || length < 0) return -1;
    if (length == 0) return 0;
    if (data == nullptr || offset > INT_MAX - length) return -1;

    int end = offset + length;
    int new_size = file.size;
    if (end > new_size) new_size = end;

    int current_blocks = file.size == 0 ? 0 : (file.size - 1) / BLOCK_SIZE + 1;
    int required_blocks = (new_size - 1) / BLOCK_SIZE + 1;
    int extra = required_blocks - current_blocks;

    if (extra > free_blocks()) return -1;

    for (int i = 0; i < extra; i++)
    {
        int block = free_block();
        if (file.last == -1)
            file.first = block;
        else
        {
            blocks[file.last].next = block;
            blocks[block].prev = file.last;
        }
        file.last = block;
    }

    if (offset > file.size)
        put_bytes(file, file.size, nullptr, offset - file.size);

    put_bytes(file, offset, data, length);
    file.size = new_size;
    return length;
}

void FileSystem::DiskState::free_chain(FileData &file)
{
    int block = file.first;
    while (block != -1)
    {
        int next = blocks[block].next;
        blocks[block].used = false;
        blocks[block].prev = -1;
        blocks[block].next = -1;
        block = next;
    }
    file.size = 0;
    file.first = -1;
    file.last = -1;
}

bool FileSystem::DiskState::check_links(const FileData &file) const
{
    int expected = file.size == 0 ? 0 : (file.size - 1) / BLOCK_SIZE + 1;
    int block = file.first;
    int prev = -1;
    int count = 0;

    while (block != -1)
    {
        if (block < 0 || block >= block_count || !blocks[block].used ||
            blocks[block].prev != prev || count >= expected)
            return false;

        prev = block;
        block = blocks[block].next;
        count++;
    }

    if (count != expected || prev != file.last) return false;

    block = file.last;
    int next = -1;
    count = 0;

    while (block != -1)
    {
        if (block < 0 || block >= block_count || !blocks[block].used ||
            blocks[block].next != next || count >= expected)
            return false;

        next = block;
        block = blocks[block].prev;
        count++;
    }

    return count == expected && next == file.first;
}

FileSystem::FileData::FileData(const std::shared_ptr<DiskState> &disk)
    : state(disk), size(0), first(-1), last(-1)
{
}

FileSystem::FileData::~FileData()
{
    state->free_chain(*this);
}

FileSystem::FileHandle::FileHandle()
    : m_file(nullptr)
{
}

FileSystem::FileHandle::FileHandle(const std::shared_ptr<FileData> &file)
    : m_file(file)
{
}

bool FileSystem::FileHandle::valid() const
{
    return m_file != nullptr;
}

int FileSystem::FileHandle::size() const
{
    if (!valid()) return -1;
    return m_file->size;
}

int FileSystem::FileHandle::read(int offset, char *data, int length) const
{
    if (!valid()) return -1;
    return m_file->state->read_bytes(*m_file, offset, data, length);
}

int FileSystem::FileHandle::write(int offset, const char *data, int length)
{
    if (!valid()) return -1;
    return m_file->state->write_bytes(*m_file, offset, data, length);
}

bool FileSystem::FileHandle::check_links() const
{
    if (!valid()) return false;
    return m_file->state->check_links(*m_file);
}

FileSystem::FileSystem(int block_count)
    : m_state(std::make_shared<DiskState>(block_count)), m_file_count(0)
{
    for (int i = 0; i < MAX_FILES; i++)
        m_files[i].name[0] = '\0';
}

FileSystem::~FileSystem()
{
}

int FileSystem::find(const char *name) const
{
    if (name == nullptr) return -1;
    for (int i = 0; i < MAX_FILES; i++)
        if (m_files[i].file != nullptr && std::strcmp(m_files[i].name, name) == 0)
            return i;
    return -1;
}

int FileSystem::free_file() const
{
    for (int i = 0; i < MAX_FILES; i++)
        if (m_files[i].file == nullptr) return i;
    return -1;
}

bool FileSystem::create(const char *name)
{
    if (name == nullptr || name[0] == '\0' || std::strlen(name) > NAME_SIZE)
        return false;
    if (find(name) != -1) return false;

    int i = free_file();
    if (i == -1) return false;

    m_files[i].file = std::make_shared<FileData>(m_state);
    std::strcpy(m_files[i].name, name);
    m_file_count++;
    return true;
}

bool FileSystem::remove(const char *name)
{
    int i = find(name);
    if (i == -1) return false;

    m_files[i].name[0] = '\0';
    m_files[i].file.reset();
    m_file_count--;
    return true;
}

bool FileSystem::rename_file(const char *old_name, const char *new_name)
{
    int i = find(old_name);
    if (i == -1 || new_name == nullptr || new_name[0] == '\0' ||
        std::strlen(new_name) > NAME_SIZE)
        return false;

    int other = find(new_name);
    if (other != -1 && other != i) return false;

    std::strcpy(m_files[i].name, new_name);
    return true;
}

FileSystem::FileHandle FileSystem::open(const char *name) const
{
    int i = find(name);
    if (i == -1) return FileHandle();
    return FileHandle(m_files[i].file);
}

int FileSystem::write(const char *name, int offset, const char *data, int length)
{
    return open(name).write(offset, data, length);
}

int FileSystem::read(const char *name, int offset, char *data, int length) const
{
    return open(name).read(offset, data, length);
}

bool FileSystem::copy(const char *source, const char *destination)
{
    int i = find(source);
    if (i == -1 || destination == nullptr || destination[0] == '\0' ||
        std::strlen(destination) > NAME_SIZE || find(destination) != -1)
        return false;

    std::shared_ptr<FileData> original = m_files[i].file;
    int required = original->size == 0 ? 0 :
                   (original->size - 1) / BLOCK_SIZE + 1;

    if (required > free_blocks() || free_file() == -1) return false;
    if (!create(destination)) return false;

    int index = find(destination);
    FileData &target = *m_files[index].file;
    target.size = original->size;

    int old_block = original->first;
    while (old_block != -1)
    {
        int block = m_state->free_block();
        std::memcpy(m_state->blocks[block].data,
                    m_state->blocks[old_block].data, BLOCK_SIZE);

        if (target.last == -1)
            target.first = block;
        else
        {
            m_state->blocks[target.last].next = block;
            m_state->blocks[block].prev = target.last;
        }

        target.last = block;
        old_block = m_state->blocks[old_block].next;
    }
    return true;
}

int FileSystem::size(const char *name) const
{
    return open(name).size();
}

int FileSystem::file_count() const
{
    return m_file_count;
}

int FileSystem::free_blocks() const
{
    return m_state->free_blocks();
}

bool FileSystem::check_links(const char *name) const
{
    return open(name).check_links();
}

void FileSystem::list() const
{
    std::cout << "Files:" << std::endl;
    for (int i = 0; i < MAX_FILES; i++)
        if (m_files[i].file != nullptr)
            std::cout << m_files[i].name << " ("
                      << m_files[i].file->size << " bytes)" << std::endl;
}
