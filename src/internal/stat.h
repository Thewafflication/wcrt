/** @file stat.h @brief Defines shared Windows file metadata. */
#ifndef WCRT_INTERNAL_STAT_H
#define WCRT_INTERNAL_STAT_H

struct wcrt_file_status {
    unsigned int device;
    unsigned long long inode;
    unsigned int mode;
    unsigned long links;
    long long size;
    long long access_time;
    long long write_time;
    long long creation_time;
};

int __wcrt_status_path(const char *path, struct wcrt_file_status *result);
int __wcrt_status_handle(void *handle, struct wcrt_file_status *result);

#endif
