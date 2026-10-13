# REQ-0090: Microsoft native-handle adoption

io.h shall declare int _open_osfhandle(__wcrt_intptr_t handle, int flags).
The pointer-sized argument shall retain native handle bits on every target.
Accepted flags are the existing _O_RDONLY/_O_WRONLY/_O_RDWR access modes,
_O_APPEND and exactly one or neither of _O_TEXT/_O_BINARY. Unknown flags,
access combination 3 and simultaneous text/binary return -1/EINVAL. Default
translation is binary. Append is supported only on disk handles; other handle
types with append return -1/EINVAL. Invalid flags are checked before handles.

The supplied handle shall be validated as disk, character or pipe; null,
INVALID_HANDLE_VALUE, closed and unknown-type handles return -1/EBADF.
Descriptor exhaustion returns -1/EMFILE. Every failure leaves caller ownership
and the native handle intact, without closing or changing its position.
Success returns an open descriptor, preserves errno and transfers ownership
of exactly the supplied handle. _close or an owning FILE's fclose closes it.
Access flags constrain WCRT reads/writes; existing native rights still apply.
Text translation and disk append reuse existing descriptor backend contracts.

Callers must supply exclusively owned handles, coordinate concurrent access/
closure and avoid adopting a handle already owned by another descriptor/stream.
For a borrowed _get_osfhandle value or a retained native copy, use Windows
DuplicateHandle and transfer ownership of the new handle. WCRT does not infer
native access rights or change inheritance/overlapped flags. Unicode modes,
unknown devices, inherited descriptor tables and historical reference defaults
remain outside this selected contract.

## Verification and impact

TC-0090 checks function/pointer widths, invalid flags/handles, translation,
read/write restrictions, append positioning, FILE/native close ownership,
native pipes, duplicates, exhaustion and slot reuse. Native/package x86/x64,
ARM64 cross-links, optional Microsoft consumers and TC-0043/0067/0070/0074
regressions apply with quality, traceability and Windows 2000 import gates.
The approved io.h inventory adds one public function; the existing private
adoption helper and its command-stream callers retain binary behavior. No
record/stream ABI, Windows import, dependency or OS baseline changes occur.
Tests use only owned handles/files. Design, risk, estimates and review are
recorded in the work log; genuine VC6 inventory/reference qualification is
Unknown, including additional flags and device errors.
