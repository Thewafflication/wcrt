/** @file fnmatch.h @brief Declares selected POSIX filename matching. */
#ifndef WCRT_FNMATCH_H
#define WCRT_FNMATCH_H

#if defined(WCRT_POSIX)
#define FNM_NOMATCH 1
#define FNM_PATHNAME 0x01
#define FNM_NOESCAPE 0x02
#define FNM_PERIOD 0x04
#define FNM_CASEFOLD 0x10

#ifdef __cplusplus
extern "C" {
#endif

int fnmatch(const char *pattern, const char *string, int flags);

#ifdef __cplusplus
}
#endif
#endif
#endif
