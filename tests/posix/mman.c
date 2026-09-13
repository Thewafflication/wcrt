/** @file mman.c @brief Exercises selected POSIX file mapping behavior. */
#define WCRT_POSIX 1
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
int main(int argc, char **argv)
{
    const char text[] = "mapped-wcrt";
    void *view;
    int descriptor;
    if (argc != 2) return 1;
    descriptor = open(argv[1], O_CREAT | O_TRUNC | O_RDWR, 0600);
    if (descriptor < 0) return 2;
    if (write(descriptor, text, sizeof(text)) != sizeof(text)) return 3;
    view = mmap(0, sizeof(text), PROT_READ, MAP_PRIVATE, descriptor, 0);
    if (view == MAP_FAILED || memcmp(view, text, sizeof(text)) != 0) return 4;
    if (munmap(view, sizeof(text)) != 0) return 5;
    if (mmap(0, 0, PROT_READ, MAP_PRIVATE, descriptor, 0) != MAP_FAILED)
        return 6;
    close(descriptor);
    return 0;
}
