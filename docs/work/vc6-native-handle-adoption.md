# Microsoft native-handle adoption tranche

## Plan and estimate

REQ-0090 adds _open_osfhandle, complementing the existing borrowed
_get_osfhandle query. Estimate: 180-350 fixture/implementation lines and
60-150 minutes. Dependencies: shared descriptor/stream ownership, existing
native file-type query and handle adoption helper. Risks: closing caller handles
on failure, double ownership, stream-slot leaks, pointer-width truncation,
translation/access flags and append position. Complete with native/package
x86/x64, ARM64 cross-links, Microsoft consumer, header inventory, error/ownership
and capacity tests, shared-I/O/command-stream regressions, quality and traceability.

## Design and review

Accept only the existing access-mode bits plus append/text/binary; reject
unknown flags, invalid access combinations and simultaneous text/binary with
EINVAL before ownership changes. Default translation is binary. Validate
disk/character/pipe handles through GetFileType, rejecting null/invalid/unknown
types with EBADF. Then allocate and initialize using the existing private
adoption helper; adjust binary/text flag without changing its existing callers.
EMFILE leaves native ownership with the caller. Success owns exactly the
supplied handle; descriptor/FILE close releases it. Caller must exclusively
own the supplied handle, use DuplicateHandle for a borrowed/already-owned
handle, and coordinate simultaneous use/closure. Native access rights remain
an OS constraint; WCRT access flags can further restrict operations.

Review no-close-before-success, pool reuse after failure, input width,
read/write restrictions, physical translation, append writes, duplicate native
ownership and FILE close. Native device/pipe handles are supported within
existing I/O contracts; overlapped I/O, inheritance changes, Unicode flags and
reference historical semantics remain out of scope. No new import, record ABI
or dependency is planned; genuine VC6 qualification remains Unknown.

## Review and verification

On 2026-10-01 native and static/DLL package fixtures passed x86 and x64 for
owned disk, pipe and NUL character handles. ARM64 direct/package fixtures
cross-linked only. Invalid flags, unknown event handles, restricted WCRT/native
access, text/binary translation, disk append, retained native duplicates,
FILE close, full pool failure and recovered slots passed. A mock retains input
bits above 32 bits on x64/ARM64 and verifies type/allocation failures occur
before ownership is adopted. The major-16 Microsoft /Od and /O2 consumers
passed invalid-handle/flag call sites; real transfer/close evidence comes from
native/package TinyCC probes, not that Microsoft failure-only addition.

TC-0016 passed 332 files, traceability passed 89 mappings and Windows 2000 x86
imports passed. Full 71-case aggregates run in the subsequent retained
checkpoint and include shared descriptor/command-stream/pool regressions.
The existing adoption helper remains binary for its original callers. Personal
review isolated the new adapter in ms_handle.c to enable input-width/error
injection without changing ownership internals. A fixture comment exceeded
80 characters and was shortened before quality verification. Roughly 310
fixture/implementation lines were added; effort was not reliably timed.
The full 71-case extension aggregates subsequently passed both native targets.
