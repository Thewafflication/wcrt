# REQ-0084: Microsoft drive queries

WCRT shall declare `int _getdrive(void)` and `unsigned long _getdrives(void)`
in `<direct.h>` without WCRT_POSIX. _getdrive returns 1–26 for an ASCII-letter
drive prefix (A–Z, case insensitive) on the process current directory. A UNC
or other non-letter prefix returns zero. Successful queries preserve errno.
The selected implementation uses the legacy MAX_PATH buffer; a failed OS
query returns zero with EACCES, and a path requiring a larger buffer returns
zero with ERANGE. No internal allocation occurs.

_getdrives returns the Windows logical-drive bitmask: bit zero identifies A,
bit 25 identifies Z. It queries drive availability without accessing the
contents of any drive, preserves errno, and returns zero on OS failure;
Windows GetLastError remains available to callers. The mask is a snapshot
and does not guarantee future access to a drive. Neither function changes
the process directory or any per-drive state.

These read-only queries are within the roadmap drive family. _chdrive,
_getdcwd, wide forms, per-drive state and long-path qualification remain
separate work. Genuine VC6 inventory/compiler/reference qualification remains
Unknown. Microsoft's [drive query](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getdrive)
and [drive mask](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getdrives)
references are supplementary.

## Verification and impact

TC-0084 verifies declaration types, native query agreement with the current
directory and logical-drive provider, unchanged cwd/errno, all ASCII drive
letters, UNC/non-letter paths and deterministic error/buffer partitions.
Native/package x86/x64 and ARM64 cross-links, TC-0064/0080 regressions,
quality, traceability and the Windows 2000 import audit apply. GetLogicalDrives
is a Windows 2000-compatible kernel32 import; existing directory interfaces,
runtime layouts and OS baseline are preserved.
