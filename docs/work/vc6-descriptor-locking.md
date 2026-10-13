# Descriptor range-locking tranche

## Plan and design

Implement REQ-0098 _locking and five sys/locking.h constants using regular-file
native handles and a physical position query. LockFile/UnlockFile accept split
64-bit offsets and a signed-long nonnegative length; no stream seek/reset or
file-length mutation is needed. Blocking modes make ten attempts on lock
contention, sleeping one second between attempts; deterministic providers
verify retries without sleeping. Other failures return immediately.

Estimate 70-100 runtime/header lines, 180-260 fixture lines and 60-100 minutes.
Review access/type/argument precedence, errno, position/state retention, split
offsets and exact unlock regions. Require native second-handle contention,
beyond-EOF locks and readonly descriptors, provider failures/retries, packaged
x86/x64 behavior, ARM64 cross-links and supplementary Microsoft calls. Retain
source-quality, traceability and Windows 2000 import gates. Additive public
symbols/header and LockFile/UnlockFile imports require rebuilt packages and
allowlist review. Existing Sleep import is retained; no host CRT dependency.
Genuine VC6 and reference-runtime qualification remain Unknown.

Microsoft's current [_locking documentation](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/locking)
informs the selected modes/error behavior. [LockFile documentation](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-lockfile)
describes immediate failure, exclusive second-handle access and beyond-EOF
regions. [UnlockFile documentation](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-unlockfile)
requires the exact original region. Current support tables start at XP and do
not establish historical introduction; record historical export evidence
separately before changing the Windows 2000 allowlist. No genuine Windows 2000
execution or differential-reference claim follows from the import gate.
Actual effort will not be fabricated.

Historical import review used Geoff Chappell's original [KERNEL32 export
research](https://www.geoffchappell.com/studies/windows/win32/kernel32/api/index.htm),
which records both LockFile and UnlockFile in NT 3.51 and later versions.
This supports adding those two longstanding imports to the Windows 2000
allowlist; it does not substitute for execution on an actual Windows 2000
installation. Current Microsoft API documentation remains the behavior source.

## Implementation and review

The presence fixture first reproduced the missing _locking declaration.
The adapter validates descriptor, arguments, disk/regular metadata and physical
position before a native range request. It captures unsigned offset halves
without seeking, closes no handles and invokes Sleep only between contention
retries. Providers verify high pointer bits, offsets 8 GiB-1 and INT64_MAX,
INT32_MAX lengths, ten attempts/nine waits, success after two waits and immediate
non-contention failures. Sleep deliberately overwrites errno; eventual success
restores the original error indicator and exhaustion sets EDEADLOCK.

Native x86/x64 checks passed independent-handle contention, wrong unlock length,
adjacent-region combined-unlock rejection, all five modes, readonly handles,
beyond-EOF wide ranges, invalid descriptors/arguments and pipe/NUL rejection.
Full FILE snapshots and position/size comparisons retain EOF/error/pushback.
Static/DLL packages passed both native targets; ARM64 compiled/linked only.
Source quality passed 364 C/H files; traceability passed 97 mappings and the
reviewed Windows 2000 import gate passed. Supplementary MSVC 16 passed all 24
standalone headers and the eleven-unit DLL consumer at /Od and /O2, including
function-pointer range lock/unlock with physical position and size retention.
All 87 retained input hashes matched the rebuilt package and current inputs.
Full native regressions are running before closeout; actual effort is untimed.

## Closeout

Both full native targets passed 15 C89 and 79 extension cases, including
controlled io.h inventory and descriptor/file-time regressions. Direct/provider
and static/DLL packages passed x86/x64; ARM64 compiled/linked only. The Windows
2000 import gate passed with the two reviewed longstanding imports. Retained
Microsoft /Od and /O2 evidence has 24 headers, eleven source units and 87 input
hashes; genuine VC6/reference and native Windows 2000 remain Unknown.
Runtime/header growth is approximately 90 lines, within estimate; fixture
growth is approximately 275 lines, modestly above estimate due to explicit
error partitions and exact adjacent-region checks. Actual effort was untimed.
