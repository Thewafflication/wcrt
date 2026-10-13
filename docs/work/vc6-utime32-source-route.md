# Explicit legacy-width file-time source route

## Plan, design and estimate

REQ-0091 adds an x86-only WCRT_VC6_UTIME32 source selector routing legacy
_utime and struct _utimbuf spellings to existing _utime32/__utimbuf32.
Estimate: 80-140 fixture/header lines and 30-75 minutes. Dependencies:
REQ-0048 corrected ranges and REQ-0086's separate metadata route. No new
runtime symbol, timestamp implementation or generic time_t ABI is introduced.
Object-like aliases support tags and function pointers consistently. Default
headers retain explicit-width APIs and absence of generic aliases; wrong
architecture selection fails visibly. Select before first header inclusion.

Review exact eight-byte record, both field offsets, function-pointer identity,
guarded input, selected/explicit status round trips and no generic time_t
narrowing. Complete with native/static/DLL x86, rejection on x64/ARM64,
Microsoft /Od-/O2 consumer, explicit-width regressions, quality and traceability.
Risks: callers assuming the selector also changes calendar functions or supplies
a binary _utime export. Document both boundaries clearly; genuine VC6 compiler/
reference qualification remains Unknown. Effort is not reliably measured.

## Review and verification

On 2026-10-01 the selected fixture passed native/static/DLL x86, including
guarded input, source function pointer, current-time selection, selected stat32
and explicit wide status/time calls. Explicit wide time one second above the
32-bit maximum remained usable and correctly caused narrow status ERANGE.
Default headers compiled on all targets; x64/ARM64 rejected the selector with
the specified diagnostic. The supplementary Microsoft major-16 DLL consumer
passed /Od and /O2, with no host CRT imports. TC-0048/0086 x86 regressions
passed. TC-0016 passed 335 files and traceability passed 90 mappings before
the final fixture initialization correction; retained final checks follow.

Microsoft /W4 /WX caught C4701 on a conservatively uninitialized status local
in the new fixture. Explicit initialization removed that test-construction
warning; control-flow errors remain checked before output is used. No runtime
function was changed or added. About 100 fixture/header lines were added,
within estimate; effort was not reliably timed. Genuine VC6 qualification is
Unknown and the source route does not imply a legacy binary/calendar ABI.
The completed 72-case extension aggregates subsequently passed x86 and x64.
