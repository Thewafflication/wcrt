/**
 * @file restrict.h
 * @brief Selects the restrict spelling for the active C language edition.
 */

#ifndef WCRT_INTERNAL_RESTRICT_H
#define WCRT_INTERNAL_RESTRICT_H

#if !defined(WCRT_C89) && !defined(__cplusplus) && \
    defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L
#define WCRT_RESTRICT restrict
#else
#define WCRT_RESTRICT
#endif

#endif
