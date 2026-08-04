/**
 * @file iso646.h
 * @brief Defines alternative spellings for C operators.
 */

#ifndef WCRT_ISO646_H
#define WCRT_ISO646_H

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199409L
#define and &&
#define and_eq &=
#define bitand &
#define bitor |
#define compl ~
#define not !
#define not_eq !=
#define or ||
#define or_eq |=
#define xor ^
#define xor_eq ^=
#endif

#endif
