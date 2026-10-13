/**
 * @file ms_types.h
 * @brief Selects 64-bit integer spellings for Microsoft compatibility APIs.
 */

#ifndef WCRT_MS_TYPES_H
#define WCRT_MS_TYPES_H

#if defined(_MSC_VER)
/** @brief Signed Microsoft-compatible 64-bit integer. */
typedef __int64 wcrt_ms_int64;
/** @brief Unsigned Microsoft-compatible 64-bit integer. */
typedef unsigned __int64 wcrt_ms_uint64;
#else
/** @brief Signed Microsoft-compatible 64-bit integer. */
typedef long long wcrt_ms_int64;
/** @brief Unsigned Microsoft-compatible 64-bit integer. */
typedef unsigned long long wcrt_ms_uint64;
#endif

#endif
