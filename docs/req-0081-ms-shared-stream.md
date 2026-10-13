# REQ-0081: Microsoft shared stream opening

WCRT shall declare `FILE *_fsopen(const char *path, const char *mode,
int sharing)` in `<stdio.h>` and provide `<share.h>` with `_SH_DENYRW=0x10`,
`_SH_DENYWR=0x20`, `_SH_DENYRD=0x30`, `_SH_DENYNO=0x40`. These Microsoft
interfaces are available in C89/C99 without WCRT_POSIX. Modes share WCRT's
existing narrow r/w/a, plus/update and text/binary stream behavior.

The sharing mode shall translate to Windows CreateFileA sharing bits:
deny read/write permits neither, deny write permits read, deny read permits
write, deny none permits both. Conflicting opens through FILE or descriptors
report EACCES. Sharing restrictions remain until the last duplicated native
handle for that open is closed. Existing fopen continues to permit both read
and write sharing. Windows symmetric access/share compatibility applies;
allowing access does not override another handle's existing sharing policy.

Success returns an allocated WCRT stream and preserves errno. Invalid null
path/mode, empty path/mode, malformed mode, or unsupported sharing values return
null with EINVAL; allocation exhaustion reports EMFILE. Windows failures map
to the existing file error vocabulary, including ENOENT for missing paths and
EACCES for denied sharing. Failed opens release their reserved stream slots.
Existing fopen now uses the same explicit backend error mapping; this is an
error-reporting correction and does not change successful I/O or sharing.
Supported mode suffixes contain at most one plus and one b/t, in either order.
Repeated/conflicting suffixes and unknown mode options report EINVAL before
opening or truncating any file. This validation also applies to fopen's shared
backend; it preserves valid C89/C99 mode strings and rejects unspecified ones.

Secure and wide forms, DOS `_SH_COMPAT`, newer `_SH_SECURE`, _sopen,
permissions/umask and a claim of complete VC6 FILE layout compatibility are
outside this slice. Default cdecl and WCRT's existing opaque FILE ABI apply.

## Verification and impact

TC-0081 checks declaration/constant values, every deny-mode/access combination,
ordinary fopen/descriptor interaction, duplication lifetime, append/read/update
behavior, argument/error partitions and repeated failure cleanup. Tests use
unique owned files. Packaged static/DLL consumers use explicit startup and
exclude host CRT imports. TC-0012/0046/0067 remain regression obligations;
TC-0079 covers share.h under the selected Microsoft compiler branch.
Microsoft's [shared stream reference](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fsopen-wfsopen)
is supplementary documentation; genuine VC6 differential qualification remains
Unknown. No runtime data layouts, Windows imports, dependencies, privileges or
minimum OS versions change. Existing API success semantics remain unchanged.
