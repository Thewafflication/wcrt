# Descriptor resize tranche plan

## Scope, dependencies and estimate

Next selected VC6 low-I/O gap: _chsize(int, long), signed 32-bit requested
length. Depend on REQ-0067 descriptor ownership/seek, REQ-0088 physical sizes
and existing Windows error mapping. Microsoft's current reference specifies
zero-filled extension and truncation, writable access and EACCES/EBADF/
ENOSPC/EINVAL failure categories:
https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/chsize
This source does not qualify genuine VC6 inventory or reference behavior.

Estimate: 90-150 adapter/backend lines, 180-300 fixture lines, 60-120 minutes.
Use the existing descriptor lookup and a private resize backend. Save the
physical position, query regular-file size, append bounded zero chunks when
extending, call baseline SetEndOfFile when truncating, and restore the original
64-bit position. Preserve FILE state and translation/append flags; raw zero
writes bypass translation and append dispatch. No _chsize_s/64-bit public
variant, generic alias, concurrency or atomic resize guarantee is planned.

Review writable access, negative lengths, regular/nonregular handles, partial
extension and failure restoration. Retain the first mutation error even if
position restoration also fails; document that failed operations can modify
size and unsuccessful restoration can leave position changed. Success must
preserve errno, existing bytes and the original physical position. Verify
small real files only; model huge boundaries and disk-full/short-write paths
with providers. No multi-gigabyte allocation is justified for this tranche.

Require controlled REQ/TC/manifest inventory, native/static/DLL x86/x64,
ARM64 compile/link, supplementary Microsoft calls, TC-0067/0088 regressions,
quality, traceability and an explicitly reviewed Windows 2000 import addition.
Source/header changes require rebuilt packages. Genuine VC6/reference status
remains Unknown; actual effort will not be invented.

## Implementation and review

The public adapter validates the live descriptor and delegates to a raw private
backend. The backend requires writable WCRT access and regular-file metadata,
saves the wide physical position, writes bounded zero chunks for extension,
uses SetEndOfFile for truncation and restores position with explicit error
precedence. The same-length case preserves errno without mutation. Append/text
dispatch is bypassed by raw WriteFile calls; FILE state is untouched.

Owned native fixtures allocate at most 1100 bytes while retaining an 8-GiB-minus-
one position, duplicate lifetime and complete FILE snapshots. Provider fixtures
cover INT_MAX lengths, INT64_MAX existing size, short/zero/oversized writes,
partial disk-full extension, denied/failed truncation, query/seek failures and
failed restoration with mutation-error precedence. The optional Microsoft
consumer uses a function pointer and reads back a zero-extended byte.

The SetEndOfFile allowlist addition was reviewed against the original
Microsoft Systems Journal April 1999 Win32 Q&A by Jeffrey Richter, retained at
https://jacobfilipp.com/MSJ/win320499.html (SetFilePointer/SetEndOfFile truncation
for existing NT/9x implementations and the Windows 2000 sparse-file discussion).
This is historical API evidence, not native Windows 2000 execution evidence.
No higher-version replacement API is used. Source quality found an 81-character
fixture comment, shortened before the final gate. No runtime defect arose in
the initial native/provider checks; failure-state behavior is explicitly bounded.

Export review confirmed _chsize and __wcrt_file_resize in all three packages.
The existing -export-all-symbols build also exports the other internal backend
helpers; "private" denotes the implementation/header contract, not hidden PE
visibility. An initial audit assumed hidden helpers and was corrected against
the retained build/export policy. This tranche does not change that policy or
remove existing helper symbols. New backend helper exposure is retained with
the matched DLL/static library, outside the supported public API inventory.

## Verification and closeout

On 2026-10-01 native/provider and static/DLL packages passed x86/x64;
ARM64 direct/provider and package consumers cross-linked only. Supplementary
Microsoft major-16 /Od and /O2 consumers passed function-pointer resize,
zero-byte extension and retained-position checks without host CRT imports.
Retained consumer input hashes matched the rebuilt runtime and current source.
Both full native aggregates passed all 75 cases, including descriptor seek/size
regressions and the controlled io.h inventory. TC-0016 passed 344 files;
traceability passed 93 mappings, Windows 2000 imports and diff checks passed.

Runtime/header growth was about 80 lines, below estimate; fixtures grew about
270 lines, within estimate. The raw zero writer is intentionally bounded to
512-byte chunks for this selected contract; large extension can take many
writes and is not claimed atomic or concurrent-safe. Actual effort was not
reliably timed. Genuine VC6/reference qualification and wider resize remain
open. The updated provider additionally verifies failed target positioning
leaves size/position unchanged without mutation or unnecessary restoration.
