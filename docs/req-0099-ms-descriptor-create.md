# REQ-0099: Microsoft descriptor creation adapter

io.h shall declare int _creat(const char *path, int permissions). Create a
missing file or truncate a writable existing file to zero, returning a WCRT
descriptor open for reading/writing with _fmode translation and deny-none
sharing. Forward permissions to the existing _sopen contract; selected mode
bits are _S_IREAD/_S_IWRITE, with the initially zero mask from REQ-0101.
All files remain readable; omission of effective write permission creates a
readonly file but its initial
read/write descriptor remains usable until close. Existing attributes are
retained; permissions apply only when creating a new file.

Preserve the existing _sopen path/flags/mode and descriptor-capacity error
behavior. Return -1 on failure and retain its errno; do not change errno after
success. Caller owns returned descriptors. A readonly existing file cannot be
truncated and shall report EACCES without changing its content. No allocation
or ownership layer beyond _sopen, host CRT or new Windows API is added.
REQ-0101 adds mutable _umask and REQ-0102 adds initially text _fmode defaults.
Wide _wcreat and POSIX creat remain outside
this scope.

## Verification and impact

TC-0099 checks the two-argument function pointer, forwarded constants/errors,
new creation, CRLF translation and mode switch, read/write access, deny-none
sharing, truncation, existing/new permissions and readonly content retention.
Native/package x86/x64, ARM64 cross-links and supplementary Microsoft calls
apply. Add declaration/runner/manifest/inventory and rebuild packages; quality,
traceability, existing _sopen regressions and imports remain gates. Work record
covers design/estimates/review. Genuine VC6/reference qualification is Unknown.
