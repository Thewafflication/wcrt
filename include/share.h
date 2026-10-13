/**
 * @file share.h
 * @brief Defines selected Win32 Microsoft file-sharing modes.
 */

#ifndef WCRT_SHARE_H
#define WCRT_SHARE_H

/** @brief Denies other read and write opens. */
#define _SH_DENYRW 0x10
/** @brief Denies other write opens. */
#define _SH_DENYWR 0x20
/** @brief Denies other read opens. */
#define _SH_DENYRD 0x30
/** @brief Permits other read and write opens. */
#define _SH_DENYNO 0x40

#endif
