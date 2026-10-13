# Microsoft system-type header tranche

## Plan and estimate

REQ-0087 supplies the missing Microsoft device, inode and file-offset type
spellings without changing existing POSIX widths or public record layouts.
Dependencies: REQ-0079 consumer probe and REQ-0086 explicit status route.
Estimate: 80-150 header/test lines and 30-75 minutes. Risks: alias collisions,
include order and accidental POSIX narrowing. Completion requires standalone
and repeated headers, type compatibility, both profile modes, packaged headers,
selected Microsoft consumers, source quality and traceability. No runtime
symbol, import, ABI migration or release qualification is introduced.

## Design and review

Always expose _dev_t as unsigned int, _ino_t as unsigned short and _off_t as
long. Expose unprefixed aliases only with WCRT_MS_TYPES_ALIASES selected and
outside WCRT_POSIX; its existing wider
ino_t/off_t remain untouched. Include sys/types.h unconditionally from
sys/stat.h to support status consumers in either include order. Do not add a
32-bit time_t alias or select a legacy calendar ABI incidentally. The current
Microsoft standard-types reference and installed major-16 header corroborate
the selected type spellings; genuine VC6 inventory remains Unknown.

Review type identity, width, signedness, repeated inclusion, caller packing,
stat32/default-stat layouts and profile isolation. Record verification below.
Effort is not reliably timed; no measured timing is claimed.

## Review finding

The first implementation exposed unprefixed aliases by default outside POSIX.
Aggregate TC-0051 caught incompatible dev_t redefinition in its strict-profile
absence fixture. This source-isolation defect was injected during header
design and removed during verification by requiring WCRT_MS_TYPES_ALIASES.
The Microsoft fixture selects this explicitly; strict mode retains absent
unprefixed names and POSIX still takes precedence. No pre-existing isolation
requirement was relaxed to make the new test pass.

## Closeout

On 2026-10-01 the corrected 68-case extension aggregates passed x86 and x64,
including strict TC-0051, default/stat32 layout and header regressions. Native
and packaged static/DLL type fixtures passed both targets; ARM64 type/profile
and packaged macro fixtures cross-linked only. The C99 floating representation
fixture compiled for ARM64. Earlier attempt to pass CompileOnly to TC-0024
used an unsupported runner option; direct compilation supplied that evidence.
TC-0016 passed 320 C/H files before the final selector correction, and
traceability passed 86 requirements/specifications/implementations. A retained
post-correction quality run follows the next descriptor tranche. Microsoft
major-16 /Od and /O2 consumers passed before/after interface expansion; final
selector-specific probe is included in subsequent retained verification.
Twenty-three standalone headers are included in that probe. Genuine VC6
qualification remains Unknown. About 80 fixture/header lines were added;
review caught the isolation defect and prevented relaxing the strict profile.
