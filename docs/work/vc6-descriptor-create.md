# Descriptor create/truncate tranche

## Plan and design

Implement REQ-0099 _creat as a two-argument adapter to _sopen: create/truncate,
read/write, text translation, deny-none sharing and selected creation modes.
Reuse REQ-0083 permissions/failure semantics rather than introduce a second
creation backend. Creation mask stays fixed at zero; mutable _umask/default
_fmode and wide _wcreat remain outside this selected contract.

Estimate 5-12 runtime/header lines, 80-130 fixture lines and 25-45 minutes.
Review exactly forwarded flags/mode, truncation, existing attribute retention,
new readonly attributes, descriptor ownership, sharing and translation state.
Require owned native create/truncate/read/write/readonly tests, deterministic
forwarding/error controls, package x86/x64, ARM64 cross-links, supplementary
Microsoft calls and quality/traceability/import gates. An additive symbol
requires rebuilt packages; no new dependency or Windows API is needed.
Genuine VC6/reference qualification remains Unknown; effort is not timed yet.

Current [Microsoft _creat documentation](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/creat-wcreat)
informs create/truncate, existing/new permission handling and deny-none sharing.
The locally installed licensed MSVC 16 crt/src/creat.c uses create/truncate/
read-write flags with deny-none. This supports the selected read/write access
choice; it is not version-12 or VC6 reference qualification, and no vendor
source is copied into WCRT. The adapter is independently a single forwarding
call with constants from WCRT's existing controlled interfaces.

## Implementation and review

The presence fixture first reproduced the missing declaration. A standalone
ms_creat.c adapter forwards one variadic _sopen call and introduces no other
ownership/error policy. Provider controls verify the exact flags/sharing/mode,
descriptor return, delegated EMFILE/EINVAL and unchanged success errno. Native
x86/x64 tests passed text CRLF bytes, mode switching, reading through the same
descriptor, deny-none independent opens, truncation, unchanged existing writable
attributes, newly readonly attributes, inability to truncate existing readonly
content, invalid-path/mode cleanup and write-only permission equivalence.
Static/DLL packages passed both native targets; ARM64 compiled/linked only.
Source quality found an 83-character fixture comment, shortened before the
gate passed 368 files with zero violations. Traceability passed 98 mappings.
The unchanged Windows 2000 import gate passed. Supplementary MSVC 16 /Od and
/O2 function-pointer create/write/read calls passed without host CRT imports;
all 87 retained consumer input hashes matched the rebuilt package and inputs.
Full native regressions are running before closeout; actual effort is untimed.

## Closeout

Both native targets passed all 15 C89 and 80 extension cases. The selected
_creat contract, direct/provider and static/DLL package checks passed x86/x64;
ARM64 compiled/linked only. Microsoft major-16 /Od and /O2 consumer evidence
and its 87 input hashes matched the rebuilt runtime. Source quality passed
368 files, traceability passed 98 mappings and import/diff gates passed.
Adapter/header growth is about 13 lines; native/provider fixtures are about
130 lines, near estimate. No vendor code was copied and no new imports were
added. Fixed zero creation mask and explicit text default remain selected
limitations; genuine VC6/reference qualification and mutable globals stay open.
