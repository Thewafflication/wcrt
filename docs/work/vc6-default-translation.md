# Default translation-mode tranche

## Plan and design

Implement REQ-0102 exported int _fmode in stdlib.h, initially _O_TEXT, with
WCRT_DLL data-import declaration. Use one snapshot for future descriptor opens
without _O_TEXT/_O_BINARY and stream opens without t/b. Explicit modes override
the default; existing descriptors and standard streams retain their modes.
Make _creat inherit this default rather than force text. Support only text and
binary values; reject an invalid default with EINVAL when it is actually used.
Binmode.obj, secure getter/setter aliases and Unicode modes remain excluded.

Estimate 30-60 runtime/header lines, 160-230 fixture lines and 60-100 minutes.
Review DLL data imports, flag/mode precedence, snapshot timing, physical CRLF
bytes, already-open modes and restoration. Require native stream/descriptor
partitions and invalid-default paths, deterministic provider snapshots, static/
DLL x86/x64, ARM64 cross-links and supplementary Microsoft direct data writes
at /Od and /O2. Retain quality/traceability/import and existing translation
regressions. Rebuild packages and update all explicitly listed file-backend
consumers for the new data module, including minimal C89/C99 and cross builds.
No new Windows import or host CRT dependency is planned. Concurrent default
updates/open transaction ordering and genuine VC6/reference remain unqualified.
Actual effort will not be fabricated.

Current [Microsoft _fmode documentation](https://learn.microsoft.com/en-us/cpp/c-runtime-library/fmode)
describes the stdlib.h global, text initial value and binary selection. WCRT's
selected value domain and invalid-default behavior are explicit requirements,
not claims of a fully frozen VC6 reference profile. Source-selected narrow
record/time flags and the default wide runtime ABI remain unchanged.

## Implementation and impact review

Added addressable int storage and the stdlib.h DLL import declaration. The
shared Windows backend validates one default snapshot only when no explicit
text/binary mode is supplied. _creat now inherits that choice. Explicit source
lists for minimal C89/C99 consumers and ARM64 cross builds include both the
creation-mask and default-mode modules. No new Windows import is required.
REQ-0082 retains independent _setmode behavior; REQ-0099 creation and the VC6
inventory now describe initially text, mutable defaults.

Review checked mode grammar, flag validation before creation, invalid defaults
before native opens, existing stream/descriptor modes, standard-stream flags,
and explicit overrides. Fixtures cover physical CRLF bytes, invalid freopen
without truncation, provider snapshot timing and exported data writes. The
Microsoft /W4 /WX build found a constant-condition warning in the width check;
replaced it with a compile-time typedef assertion, then both /Od and /O2 passed.
Quality scans 380 C/header files with zero violations; traceability covers 101
requirements/specifications/implementations. Packaged focused x86/x64 tests
pass; ARM64 focused cases compile/link only. Windows 2000 x86 imports pass.
Genuine VC6/reference, native ARM64 and concurrent default updates remain
unqualified. Actual effort is untimed. Full-suite closeout is pending.

## Closeout

Both native targets passed 15 C89 and 83 extension cases, including creation,
shared opens, _setmode and the new default-mode partitions. All 15 ARM64 C89
cases cross-built; execution remains deferred. Supplementary MSVC 16 consumers
passed /Od and /O2 with 24 standalone headers, 12 source units, no host CRT
imports and 88 matching retained input hashes. Runtime/header/backend growth
is about 35 lines; focused fixtures and imported-data consumer total about
210 lines, within estimate. Quality/traceability/import/diff checks passed.
Genuine VC6/reference and concurrent default updates remain Unknown. Actual
effort was untimed. No release or legacy binary-ABI gate is closed by this work.
