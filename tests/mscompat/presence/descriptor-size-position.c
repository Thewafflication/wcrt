/** @file descriptor-size-position.c @brief Checks descriptor query types. */
#include <io.h>
#include <io.h>

wcrt_ms_int64 (*wide_seek)(int, wcrt_ms_int64, int) = _lseeki64;
wcrt_ms_int64 (*wide_tell)(int) = _telli64;
wcrt_ms_int64 (*wide_length)(int) = _filelengthi64;
long (*narrow_length)(int) = _filelength;
