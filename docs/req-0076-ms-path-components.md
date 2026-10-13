# REQ-0076: Microsoft narrow path components

WCRT shall expose `_makepath` and `_splitpath` in `<stdlib.h>` in C89 and
C99 modes, without requiring `WCRT_POSIX`. They are Microsoft compatibility
interfaces, not ISO C functions. The header shall provide `_MAX_PATH=260`,
`_MAX_DRIVE=3`, `_MAX_DIR=256`, `_MAX_FNAME=256`, and `_MAX_EXT=256`.

`_makepath(path, drive, directory, filename, extension)` shall construct a
terminated byte string. A nonempty drive contributes its first byte and a
colon. A nonempty directory is copied verbatim and receives a trailing
backslash unless it already ends in a slash or backslash. The filename is
copied verbatim. A nonempty extension receives a leading period unless it
already starts with one. Null and empty component pointers omit components.
No filesystem lookup, normalization, validation, allocation, or case folding
shall occur. The caller supplies sufficient output space; input/output overlap
is outside the contract. A null output sets `errno=EINVAL` and returns.

`_splitpath(path, drive, directory, filename, extension)` shall split bytes
lexically. A colon in the second position identifies the two-byte drive. The
directory extends through the last slash or backslash after the drive. The
last period after that separator starts the extension, including for a leading
or trailing period. Missing components yield empty strings. Each output may
be null to omit it. UNC prefixes remain part of the directory; there is no
special UNC drive component. Caller-owned buffers must fit their component
including its terminator; the `_MAX_*` constants describe the selected legacy
buffer model and do not enable bounds checking. Outputs shall not overlap
one another or the input. Null input clears requested outputs and sets EINVAL.
Successful calls preserve `errno`.

## Baseline and qualification

This is an additive slice of the selected VC6-era path family in the 1.1.0
roadmap. Historical Microsoft documentation identifies these interfaces in
the pre-VC6 runtime; the currently published
[split documentation](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/splitpath-wsplitpath)
and [construction documentation](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/makepath-wmakepath)
provide supplementary interface references. WCRT specifies its C-locale byte
contract explicitly; multibyte code-page parsing, wide and secure variants,
and invalid-parameter handlers are outside this tranche. Genuine VC6 compiler,
header/ABI and differential execution evidence remains Unknown until run in a
licensed retained VC6 environment. Passing TinyCC tests do not establish it.

## Verification and impact

TC-0076 tests declarations, constants, reverse decomposition partitions,
omitted components, separator and period boundaries, long components, guard
bytes, errno, and errors on native x86/x64 and ARM64 where available. Both
static and DLL builds include the lexical implementation. No new Windows
imports, public structure layouts, existing symbols, minimum OS requirements,
dependencies, privileges or filesystem writes are introduced.
