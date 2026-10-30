#include "FileSystem.h"


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
    FileSystem disk(20);
    check(disk.file_count() == 0, "empty disk");
    check(disk.free_blocks() == 20, "all blocks are free");
    check(!disk.create(nullptr), "null filename");
    check(!disk.create(""), "empty filename");
    check(!disk.create("12345678901234567890123456789012"), "long filename");
    check(disk.create("a"), "create file");
    check(!disk.create("a"), "duplicate filename");
    check(disk.size("a") == 0, "new file size");
    check(disk.check_links("a"), "empty file links");

    check(disk.write("a", 0, "Hello", 5) == 5, "write first bytes");
    char buffer[300] = {};
    check(disk.read("a", 0, buffer, 5) == 5 && std::memcmp(buffer, "Hello", 5) == 0,
          "read first bytes");
    check(disk.size("a") == 5, "file size after write");

    char big[130];
    for (int i = 0; i < 130; i++) big[i] = 'A' + i % 26;
    check(disk.write("a", 5, big, 130) == 130, "write across several blocks");
    check(disk.size("a") == 135, "file size across blocks");
    check(disk.check_links("a"), "forward and backward links");
    check(disk.read("a", 5, buffer, 130) == 130 && std::memcmp(buffer, big, 130) == 0,
          "read across blocks");

    check(disk.copy("a", "b"), "copy file");
    check(disk.size("b") == 135, "copy size");
    check(disk.check_links("b"), "copy links");
    check(disk.read("b", 0, buffer, 5) == 5 && std::memcmp(buffer, "Hello", 5) == 0,
          "copy contents");
    check(!disk.copy("a", "b"), "copy to an existing filename");
    check(!disk.copy("absent", "c"), "copy nonexistent file");

    check(disk.write("a", 0, "Bye", 3) == 3, "overwrite first bytes");
    check(disk.read("b", 0, buffer, 5) == 5 && std::memcmp(buffer, "Hello", 5) == 0,
          "copy is independent");

    check(!disk.rename_file("a", "b"), "rename to existing name");
    check(disk.rename_file("a", "a"), "rename to same name");
    check(disk.rename_file("a", "new"), "rename file");
    check(disk.size("a") == -1 && disk.size("new") == 135, "old name removed");
    check(!disk.rename_file("a", "other"), "rename nonexistent file");
    check(!disk.rename_file("new", ""), "rename to empty name");

    char bytes[4] = {'X', '\0', 'Y', 'Z'};
    check(disk.write("new", 200, bytes, 4) == 4, "write with gap");
    check(disk.size("new") == 204, "file size after gap");
    check(disk.read("new", 135, buffer, 65) == 65, "read gap");
    bool zeros = true;
    for (int i = 0; i < 65; i++)
        if (buffer[i] != 0) zeros = false;
    check(zeros, "gap filled with zero bytes");
    check(disk.read("new", 200, buffer, 4) == 4 && std::memcmp(buffer, bytes, 4) == 0,
          "binary data including zero byte");
    check(disk.check_links("new"), "links after gap");

    check(disk.read("new", 204, buffer, 10) == 0, "read at end of file");
    check(disk.read("new", 202, buffer, 10) == 2, "short read at end");
    check(disk.read("new", -1, buffer, 1) == -1, "negative read offset");
    check(disk.read("new", 0, nullptr, 1) == -1, "null read buffer");
    check(disk.read("absent", 0, buffer, 1) == -1, "read nonexistent file");
    check(disk.write("new", -1, "!", 1) == -1, "negative write offset");
    check(disk.write("new", 0, nullptr, 1) == -1, "null write buffer");
    check(disk.write("absent", 0, "!", 1) == -1, "write nonexistent file");
    check(disk.write("new", INT_MAX, "!?", 2) == -1, "offset overflow");
    check(disk.write("new", 300, nullptr, 0) == 0 && disk.size("new") == 204,
          "zero-byte write");

    check(disk.remove("new"), "delete file");
    check(!disk.remove("new"), "delete nonexistent file");
    check(disk.remove("b"), "delete copied file");
    check(disk.file_count() == 0 && disk.free_blocks() == 20, "all blocks released");
    check(disk.create("new"), "reuse filename");
    check(disk.write("new", 0, "ok", 2) == 2, "reuse blocks");
    check(disk.read("new", 0, buffer, 2) == 2 && std::memcmp(buffer, "ok", 2) == 0,
          "read reused blocks");

    FileSystem small(2);
    char full[128];
    for (int i = 0; i < 128; i++) full[i] = 'x';
    check(small.create("full"), "small disk file");
    check(small.write("full", 0, full, 128) == 128, "fill disk exactly");
    check(small.free_blocks() == 0, "no free blocks");
    check(small.write("full", 128, "!", 1) == -1, "write to full disk");
    check(small.size("full") == 128 && small.check_links("full"),
          "failed write did not damage file");
    check(!small.copy("full", "copy"), "copy when disk is full");
    check(small.file_count() == 1, "failed copy did not create file");
    check(small.remove("full") && small.free_blocks() == 2, "recover full disk space");

    FileSystem directory(0);
    bool directory_ok = true;
    for (int i = 0; i < MAX_FILES; i++)
    {
        char name[4] = {'f', (char)('0' + i / 10), (char)('0' + i % 10), '\0'};
        if (!directory.create(name)) directory_ok = false;
    }
    check(directory_ok && directory.file_count() == MAX_FILES, "fill directory");
    check(!directory.create("extra"), "full directory");
    check(directory.remove("f42") && directory.create("extra"), "reuse directory slot");
    check(directory.free_blocks() == 0, "directory does not consume data blocks");

    // One file can have several owners: directory and open handles.
    FileSystem shared(5);
    check(shared.create("shared"), "create shared file");
    FileSystem::FileHandle first = shared.open("shared");
    FileSystem::FileHandle second = first;
    check(first.valid() && second.valid(), "two open handles");
    check(first.write(0, "abc", 3) == 3, "write using first handle");
    check(second.read(0, buffer, 3) == 3 && std::memcmp(buffer, "abc", 3) == 0,
          "second handle sees changes");

    check(shared.remove("shared"), "unlink open file");
    check(shared.file_count() == 0 && shared.size("shared") == -1 &&
          !shared.open("shared").valid(), "unlinked name is not visible");
    check(shared.free_blocks() == 4, "unlinked open file keeps blocks");
    check(second.write(5, "Z", 1) == 1 && first.size() == 6,
          "write to unlinked file");
    check(first.read(0, buffer, 6) == 6 && buffer[3] == 0 &&
          buffer[4] == 0 && buffer[5] == 'Z', "read unlinked file and gap");
    check(first.check_links() && second.check_links(), "unlinked file links");

    check(shared.create("shared"), "reuse unlinked name");
    check(shared.write("shared", 0, "new", 3) == 3, "write new file with reused name");
    check(first.read(0, buffer, 3) == 3 && std::memcmp(buffer, "abc", 3) == 0,
          "old handle still points to old file");
    check(shared.read("shared", 0, buffer, 3) == 3 &&
          std::memcmp(buffer, "new", 3) == 0, "new named file independent");
    check(shared.free_blocks() == 3, "old and new files each use a block");

    first = FileSystem::FileHandle();
    check(shared.free_blocks() == 3, "one remaining handle keeps old blocks");
    second = FileSystem::FileHandle();
    check(shared.free_blocks() == 4, "last handle releases old blocks");
    check(shared.remove("shared") && shared.free_blocks() == 5,
          "named file releases blocks");

    FileSystem::FileHandle saved;
    {
        FileSystem temporary(3);
        check(temporary.create("temporary"), "create temporary disk file");
        saved = temporary.open("temporary");
        check(saved.write(0, "live", 4) == 4, "write before disk destruction");
    }
    check(saved.valid() && saved.read(0, buffer, 4) == 4 &&
          std::memcmp(buffer, "live", 4) == 0,
          "open handle outlives FileSystem");
    check(saved.write(70, "?", 1) == 1 && saved.size() == 71 &&
          saved.check_links(), "handle writes after FileSystem destruction");
    saved = FileSystem::FileHandle();
    check(!saved.valid() && saved.read(0, buffer, 1) == -1 &&
          saved.write(0, "x", 1) == -1 && saved.size() == -1,
          "invalid handle methods");

    std::cout << "Enter number of random tests: ";
    int n;
    if (!(std::cin >> n) || n < 0 || n > 100000)
    {
        std::cout << "Incorrect number of tests" << std::endl;
        return 1;
    }

    FileSystem random_disk(20);
    random_disk.create("test");
    char expected[512] = {};
    char actual[512] = {};
    int expected_size = 0;

    std::srand(12345);

    for (int t = 0; t < n; t++)
    {
        char data[80];
        int offset = std::rand() % 400;
        int length = std::rand() % 80 + 1;

        for (int j = 0; j < length; j++)
            data[j] = (char)(std::rand() % 256);

        int result = random_disk.write("test", offset, data, length);
        if (offset > expected_size)
            std::memset(expected + expected_size, 0, offset - expected_size);
        std::memcpy(expected + offset, data, length);
        if (offset + length > expected_size)
            expected_size = offset + length;

        int received = random_disk.read("test", 0, actual, 512);
        check(result == length && received == expected_size &&
              std::memcmp(actual, expected, expected_size) == 0 &&
              random_disk.check_links("test"), "random write/read/links");
    }

    std::cout << "Passed: " << passed << " / " << total << std::endl;
    if (passed == total)
        std::cout << "All tests passed." << std::endl;

    return passed == total ? 0 : 1;
}
