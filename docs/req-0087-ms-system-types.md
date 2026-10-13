# REQ-0087: Microsoft system-type spellings

WCRT sys/types.h shall declare _dev_t as unsigned int, _ino_t as unsigned
short and _off_t as long, respectively unsigned 32-bit, unsigned 16-bit and
signed 32-bit on all supported Windows targets. With WCRT_MS_TYPES_ALIASES
selected before first inclusion and outside WCRT_POSIX, it shall also declare
dev_t, ino_t and off_t as aliases of those types. Without either selector,
these unprefixed types shall remain absent. With WCRT_POSIX
selected, existing dev_t, ino_t and off_t types and widths shall remain
unchanged; underscored Microsoft types remain independently available.

sys/stat.h shall expose these declarations through sys/types.h regardless of
profile. Repeated headers and either inclusion order shall be valid. Existing
stat32, stat64 and POSIX status layouts, time_t, runtime signatures, exports
and supported architecture contracts shall remain unchanged. These source
types alone do not qualify historical device-number or file-offset behavior.

## Verification and impact

TC-0087 checks exact compatible types, signedness/widths, both include orders,
repeated headers, both profile modes and selected Microsoft x86 consumers.
TC-0051 remains a regression for strict-profile unprefixed type absence.
Native x86/x64, packaged headers and ARM64 cross-compilation apply alongside
TC-0049/0079/0086, source quality and traceability. Header-only source additions
need a rebuild to consume; no new runtime dependencies, Windows imports,
security controls or binary-layout changes arise. Schedule is recorded in
the work log. Genuine VC6 compiler/reference qualification remains Unknown.
