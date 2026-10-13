# REQ-0085: Microsoft drive directory adapters

WCRT shall declare `int _chdrive(int drive)` and
`char *_getdcwd(int drive, char *buffer, int size)` in `<direct.h>` without
WCRT_POSIX. Drive numbers 1–26 identify A–Z; _getdcwd additionally accepts
zero for the current directory, including a current UNC directory.

_chdrive asks Windows to select the requested drive using its drive-relative
directory state. It returns zero and preserves errno on success, or -1 with
EACCES for invalid/unavailable drives or a denied change. Failed changes leave
cwd unchanged. Selecting the current drive retains its current directory.
Callers coordinate cwd mutation and drive-relative operations.

_getdcwd queries without changing cwd. Size must be positive; zero/negative
sizes return NULL with EINVAL. Invalid/unavailable drive numbers return NULL
with EACCES. Drive zero or the current letter drive delegates to _getcwd;
another available drive resolves its Windows drive-relative dot path. A
caller buffer receives a terminated result and the same pointer is returned.
A NULL buffer allocates size bytes, released by the caller with free. A result
that exceeds the supplied size reports ERANGE, allocation failure ENOMEM,
and other Windows path-query failures EACCES. Success preserves errno.

This selected profile uses Windows-provided/inherited per-drive state. It
does not add CRT per-drive environment bookkeeping to _chdir (excluded by
REQ-0080), a mutable per-drive cache, locks, wide paths or long-path
qualification. Remembered directories on inactive drives are therefore an
explicit reference-profile gap. Genuine VC6 inventory/compiler/reference
qualification remains Unknown. Microsoft's
[drive change](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/chdrive)
and [drive directory](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getdcwd-wgetdcwd)
references are supplementary.

## Verification and impact

TC-0085 checks declarations, current/default-drive queries, caller/allocated
buffers, exact size/range errors, invalid drives/sizes, unchanged query cwd
and current-drive selection on an owned directory. Deterministic OS
substitutes cover other-drive resolution and switch failures without accessing
real alternate drives. Native/package x86/x64, ARM64 cross-links, directory
regressions, quality, traceability and Windows 2000 imports apply. No new
Windows imports or runtime layouts are introduced. Existing directory/path
contracts and explicit bookkeeping exclusions remain unchanged.
