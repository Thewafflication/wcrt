# WPM TinyCC Compatibility Inventory

**Content type:** Compatibility source inventory

**Status:** Baselined for the REQ-0050/REQ-0051 integration slice

**Source baseline:** WPM branch `codex/fix-windows-xp-kernel32-errors` at
`ef2c4c5`; final pre-removal state at parent of `252b8b0`

## Purpose

The user-referenced `wpm/tcc/_compatibility` directory is represented in the
available WPM history as `wpm/tcc_compat`. WPM removed its runtime files after
adopting WCRT. This inventory classifies every former file and callable
interface so integration does not confuse ISO C, Microsoft CRT, POSIX, Windows
API, compiler support, and application-local helpers.

## Callable Interface Disposition

| Former interface | Origin | WCRT disposition | Controlled evidence |
| --- | --- | --- | --- |
| `snprintf` | ISO C99, 7.19.6.5 | Implemented as the complete ISO function; WPM's limited formatter is not copied | REQ-0019, REQ-0029; TC-0019, TC-0029 |
| `_stricmp`, `_strnicmp` | Microsoft CRT | Implemented | REQ-0045; TC-0045 |
| `fopen_s`, `sscanf_s` | Microsoft secure CRT | Implemented | REQ-0046; TC-0046 |
| `strcpy_s`, `strncpy_s` | Microsoft secure CRT | Implemented | REQ-0047; TC-0047 |
| `_utime32`, `_utime64` | Microsoft CRT | Implemented | REQ-0048; TC-0048 |
| `_stat64`, x86 `_stat` | Microsoft CRT plus a verified TinyCC x86 linker condition | Implemented | REQ-0049; TC-0049 |
| console `_start` | Windows PE process startup using `GetCommandLineA` and `ExitProcess` | Replaced by WCRT's separately linked startup object | REQ-0017; TC-0017 |
| `stat` | POSIX | Implemented as a selected, typed Windows adapter | REQ-0051; TC-0051 |
| `utime` | POSIX | Implemented as a selected, typed Windows adapter | REQ-0051; TC-0051 |
| `__iob_func` | TinyCC bridge to a private MSVCRT stream-table detail | Excluded: it applied only to the non-WCRT XP/MSVCRT path and would not preserve WCRT `FILE` ownership | ADR-0007; 1.1.0 roadmap private-export exclusion |

The ISO classification uses the public
[C99 committee draft](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf).
Microsoft classifications use Microsoft Learn documentation for
[`_stricmp`/`_strnicmp`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/stricmp-wcsicmp-mbsicmp-stricmp-l-wcsicmp-l-mbsicmp-l),
[`fopen_s`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-s-wfopen-s),
[`sscanf_s`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/sscanf-s-sscanf-s-l-swscanf-s-swscanf-s-l),
[`strcpy_s`/`strncpy_s`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strcpy-s-wcscpy-s-mbscpy-s),
the [`_utime` family](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/utime-utime32-utime64-wutime-wutime32-wutime64),
and the [`_stat` family](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/stat-functions).
The POSIX classifications use the Open Group specifications for
[`stat`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/stat.html)
and
[`utime`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/utime.html).

## Header and Constant Disposition

| Former definition | Origin | Disposition |
| --- | --- | --- |
| `ULLONG_MAX` | ISO C99 | Already implemented by REQ-0023 |
| `errno` mapped to `wcrt_errno` | ISO C error indicator plus WCRT storage ABI | Already implemented by REQ-0003 |
| `EACCES`, `EINVAL`, `ENOMEM` | Microsoft CRT and POSIX shared names | Already implemented for WCRT compatibility operations |
| `EAGAIN`, `EFBIG`, `EINTR`, `EIO`, `ENOSYS`, `ENXIO`, `EPERM` | POSIX names with Microsoft/TinyCC Windows values | Implemented under `WCRT_POSIX` by REQ-0050 |
| `_TIME_T_DEFINED` | Microsoft/TinyCC header interoperability guard | Defined by WCRT after its `time_t` declaration |
| `stat`/`utime` preprocessor aliases | WPM application-local bridge | Replaced by typed functions under ADR-0007 |
| ACL and URL Moniker declarations and `.def` files | Windows API and TinyCC SDK supplement | OS/compiler support, not C-runtime functionality; retained by WPM as `tcc_support` |

POSIX defines the error symbols but does not assign their numeric values. WCRT
uses the Windows compiler/CRT values documented by Microsoft in
[`errno` constants](https://learn.microsoft.com/en-us/cpp/c-runtime-library/errno-constants)
and verified against the selected TinyCC Windows headers. This intentionally
corrects WPM's local `ENOSYS` value from 38 to the Windows value 40.

## Compatibility Limits

- New POSIX names require `WCRT_POSIX`; `_POSIX_VERSION` is not defined.
- `stat` reports the Windows metadata mapping controlled by REQ-0051, not a
  native Unix filesystem identity or permission model.
- `utime` inherits WCRT's representable Microsoft 64-bit time range and error
  translation; negative timestamps are rejected with `EINVAL`.
- `__iob_func` is not a public standard function and cannot safely bridge
  WCRT's `FILE` objects to an MSVCRT-owned stream table.
- Windows SDK declarations remain outside WCRT even when the TinyCC SDK needs
  a supplemental header or import definition.
