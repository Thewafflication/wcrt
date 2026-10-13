/** @file find32.c @brief Tests an owned native ANSI filename search. */
#include <io.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>

/** @brief Enumerates exactly the owned matching file without assuming order. */
int main(int argc, char **argv)
{
    struct guarded {
        unsigned long before;
        struct wcrt_finddata32 data;
        unsigned long after;
    } value;
    struct wcrt_finddata32 saved;
    __wcrt_intptr_t handle;
    __wcrt_intptr_t second;
    FILE *file;
    int seen = 0;
    if (argc != 3 || sizeof(value.data) != 280 ||
        offsetof(struct wcrt_finddata32, name) != 20) return 1;
    file = fopen(argv[1], "wb");
    if (!file) return 2;
    if (fwrite("abc", 1, 3, file) != 3 || fclose(file)) return 3;
    value.before = 0x12345678UL;
    value.after = 0xabcdef01UL;
    errno = 123;
    if (strlen(argv[2]) >= sizeof(value.data.name)) return 12;
    strcpy(value.data.name, argv[2]);
    handle = wcrt_findfirst32(value.data.name, &value.data);
    if (handle == -1 || errno != 123) return 4;
    second = wcrt_findfirst32(argv[2], &value.data);
    if (second == -1 || second == handle || wcrt_findclose(second)) return 11;
    do {
        if (value.data.size != 3 || !strstr(value.data.name, "owned-"))
            return 5;
        ++seen;
        saved = value.data;
        errno = 123;
    } while (!wcrt_findnext32(handle, &value.data));
    if (errno != ENOENT || seen != 1 ||
        memcmp(&value.data, &saved, sizeof(saved)) ||
        value.before != 0x12345678UL ||
        value.after != 0xabcdef01UL) return 6;
    errno = 123;
    if (wcrt_findclose(handle) || errno != 123) return 7;
    if (wcrt_findclose(handle) != -1 || errno != EINVAL) return 8;
    if (remove(argv[1])) return 9;
    if (wcrt_findfirst32(argv[2], &value.data) != -1 || errno != ENOENT)
        return 10;
    return 0;
}
