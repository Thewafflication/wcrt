# REQ-0091: Explicit legacy-width file-time source route

With WCRT_VC6_UTIME32 selected before first sys/utime.h inclusion, x86
consumers shall receive object-like aliases _utime to _utime32 and _utimbuf
to __utimbuf32. This preserves structure-tag and function-pointer source use.
The selected record remains eight bytes, signed 32-bit actime/modtime at offsets
zero/four. Existing explicit 32-/64-bit declarations remain available and retain
their ABI, including the 16-byte __utimbuf64 record. WCRT time_t remains 64-bit;
this selector does not select a generic calendar-time ABI or add a binary
_utime export. Wrong-target selection on x64/ARM64 shall fail compilation.

REQ-0093 extends this same opt-in selector with _futime to _futime32. It reuses
the selected record and retains absent default aliases and binary generic
exports; descriptor behavior is controlled by the separate requirement.

Default headers shall retain absent generic _utime/_utimbuf aliases. Selection
shall compose with WCRT_VC6_STAT32 without changing explicit wide metadata or
file-time calls. Selected behavior/errors reuse REQ-0048, including current-time
range validation; the narrow record must not be passed to generic calendar
functions through pointer casts. Copy its timestamp into time_t when needed.

## Verification and impact

TC-0091 checks default absence, exact widths/offsets, function pointers, guarded
input, selected status round trips, null-current updates and explicit wide
calls in selected translation units. Native/static/DLL x86 and supplementary
Microsoft /Od-/O2 consumers apply; other targets test visible selector rejection
and existing explicit interfaces. TC-0048/0086, quality and traceability remain
gates. Header-only routing needs an opt-in/rebuild; no runtime symbol, binary
layout, Windows import, dependency or supported-target change occurs. The work
record covers review/risk/schedule. Genuine VC6 compiler/service-pack/reference
qualification remains Unknown; full legacy time/device ABI is separate work.
