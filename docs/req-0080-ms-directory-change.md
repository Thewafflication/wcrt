# REQ-0080: Microsoft narrow directory change and removal

WCRT shall declare `_chdir(const char *path)` and `_rmdir(const char *path)`
in `<direct.h>` in C89/C99 modes without a POSIX selector. Each returns zero
on success, preserves errno on success, and returns -1 with errno on failure.
Null input reports EINVAL. Wide, secure and POSIX forms and CRT per-drive
environment bookkeeping are not introduced by this slice.

`_chdir` shall change the process current directory through the Windows narrow
path API, accepting existing absolute and relative directories including dot
segments. Changing to another drive also changes the default drive. Missing
paths and file inputs report ENOENT; denied or shared access reports EACCES.
Failure leaves the directory unchanged. The process cwd is shared state;
callers coordinate concurrent directory changes and relative-path operations.

`_rmdir` shall remove an empty directory, without recursion or file deletion.
Nonempty directories, file inputs, the current directory, and drive/UNC share
roots report ENOTEMPTY. Missing/invalid paths report ENOENT; denied or shared
access reports EACCES. Current/root preclassification uses canonical paths in
the legacy MAX_PATH model and prevents attempting their deletion. Paths beyond
that model receive the underlying Windows result; extended-length aliases,
reparse-point equivalence and per-directory case-sensitive namespaces are not
qualified by this tranche. Successful removal does not change the cwd.

## Verification and impact

TC-0080 covers independent declarations, owned absolute/relative paths,
dot segments, cwd restoration, null/missing/file paths, nonempty/current
directories and errno preservation. It deletes only its own fixtures.
Deterministic OS substitutes verify drive/UNC root and current-directory
protection without issuing any actual removal/change operation, including
missing UNC shares and access/sharing error mapping.
TC-0079 covers the updated direct.h C-consumer surface; TC-0064 remains the
directory regression gate. The contemporary Microsoft
[change reference](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/chdir-wchdir?view=msvc-170)
and [removal reference](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/rmdir-wrmdir?view=msvc-170)
are supplementary documentation; genuine VC6 qualification remains Unknown.

Two Windows 2000 kernel32 APIs, SetCurrentDirectoryA and RemoveDirectoryA,
join the x86 import allowlist. Existing public structures, allocation and
symbols retain their ABI; no privilege, dependency or OS baseline changes.
Directory removal is caller-requested and nonrecursive; no process startup
or automatic runtime cleanup invokes it.
