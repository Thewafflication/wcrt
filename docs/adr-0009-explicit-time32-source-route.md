# ADR-0009: Explicit 32-bit calendar-time source route

## Context and decision

WCRT's existing time, mktime, difftime, gmtime, localtime and ctime symbols use
signed 64-bit time_t. Replacing their ABI with VC6-width pointers/results would
break shipped callers. Add distinct default-cdecl wcrt_time32, wcrt_mktime32,
wcrt_difftime32, wcrt_gmtime32, wcrt_localtime32 and wcrt_ctime32 interfaces,
using Windows signed 32-bit long timestamps. Preserve all existing symbols.

WCRT_VC6_TIME32 before first time.h inclusion selects x86 time_t as long and
object-like aliases for these six generic C spellings. Other declarations,
clock_t, struct tm, fpos_t and explicit-width status/file-time records remain
unchanged. Reject selection on x64/ARM64 and with WCRT_POSIX: the shipped POSIX
timeval/timespec/stat layouts use wide timestamps and cannot share this route.
Consumers rebuild with the selector; the runtime itself retains its wide ABI.

Bound narrow calendar inputs/results to nonnegative Unix seconds through
2147483647. Numeric difftime accepts the full signed timestamp pair and computes
in double without signed subtraction overflow. Current-time failure stores -1
in a supplied narrow output. mktime uses a guarded copy, checked normalization
and narrow-result validation before committing caller fields. Preserve the
existing shared conversion/text storage and Windows timezone policy, including
indeterminate tm_isdst. Generic binary legacy exports are not replaced, and
this does not claim exact VC6 timezone/error/static-storage behavior.

## Consequences and verification

Versioned names make the source ABI choice explicit, as in ADR-0008's metadata
route. Default C89/POSIX and shipped consumers retain 64-bit timestamps; the
narrow source route composes with WCRT_VC6_STAT32/WCRT_VC6_UTIME32. Test widths,
pointer signatures, guarded buffers, epoch/2038 bounds, error/no-mutation paths,
normalization, local round trips, source include orders and wrong-profile
rejection. A deterministic clock/provider fixture must verify future/pre-epoch
cases without changing the system clock. Native/package x86 and supplementary
Microsoft calls apply; explicit adapters compile/link on all targets. Genuine
VC6 compiler/reference qualification remains Unknown.
