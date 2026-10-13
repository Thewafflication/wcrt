# REQ-0083: Microsoft shared descriptor opening

WCRT shall declare `int _sopen(const char *path, int flags, int sharing, ...)`
in `<io.h>` without WCRT_POSIX. It shall support the existing _open access,
append, create/truncate/exclusive and text/binary flags, with the four
share.h deny modes from REQ-0081. A pmode argument is required only with
_O_CREAT; it contains only _S_IREAD and/or _S_IWRITE (zero is also accepted).
Omitting a required argument is outside the contract. The selected profile
initializes the creation mask to zero; REQ-0101 adds mutable _umask snapshots.

Success returns a WCRT descriptor and preserves errno. Failed opens release
their reserved slots. Invalid path, flags, sharing or creation permission
bits return -1 with EINVAL; descriptor exhaustion reports EMFILE. Missing
paths report ENOENT, conflicting sharing or denied access EACCES, and
_O_CREAT|_O_EXCL on an existing path EEXIST. Invalid arguments shall be
rejected before file creation or truncation. pmode is not read without
_O_CREAT and does not change existing file attributes.

A newly created file without effective _S_IWRITE after mask application shall
acquire the Windows read-only
attribute before the descriptor is returned; with _S_IWRITE it remains
writable. Windows does not represent write-only files. The initial descriptor
retains its requested access rights; the read-only attribute restricts later
opens. Callers coordinate the path namespace during creation and attribute
application. If setting creation permissions fails, the handle and descriptor
slot are released, but the newly created file may remain.

Sharing rules, duplicate lifetime and narrow translation match REQ-0081 and
REQ-0067. Existing _open and POSIX open creation-permission behavior remains
unchanged. This is an explicitly selected _sopen slice, not full VC6 low-I/O
qualification; wide/secure variants, extended open flags and Unicode
modes are excluded.

## Verification and impact

TC-0083 verifies sharing/access combinations, duplicate lifetime, argument and
error partitions, repeated failures, exclusive/create/truncate/append behavior,
translation and new-versus-existing creation permissions. Tests use owned
files and restore read-only attributes before removal. Native/package
x86/x64 and ARM64 cross-links, TC-0081/0067 regressions, traceability, source
quality and Windows 2000 import checks apply. Existing Windows attribute
imports are reused; runtime structures and minimum OS do not change.
Microsoft's [shared open reference](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/sopen-wsopen)
is supplementary; genuine VC6 reference qualification remains Unknown.
REQ-0101 applies one creation-mask snapshot after requested-mode validation.
TC-0101 verifies mask/new/existing partitions and initial descriptor access;
TC-0083 remains the default-zero sharing/permission regression. Rebuilt packages
are required; no new Windows import or existing record layout is introduced.
