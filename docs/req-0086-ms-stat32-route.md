# REQ-0086: Explicit 32-bit file-status source route

WCRT shall provide `struct wcrt_stat32` and the cdecl interfaces
`int wcrt_stat32(const char *path, struct wcrt_stat32 *result)` and
`int wcrt_fstat32(int descriptor, struct wcrt_stat32 *result)` in sys/stat.h.
The record uses unsigned 32-bit st_dev/st_rdev; unsigned 16-bit st_ino/st_mode;
signed 16-bit st_nlink/st_uid/st_gid; and signed 32-bit st_size/st_atime/st_mtime/
st_ctime. Its size is 36 bytes, with offsets 0,4,6,8,10,12,16,20,24,28,32,
respectively, under the header's eight-byte packing scope. Caller packing
shall be restored afterwards.

The explicit x86-only WCRT_VC6_STAT32 header selector shall route _stat's
function and structure tag to wcrt_stat32 and _fstat to wcrt_fstat32.
Selection must precede first header inclusion. The selector on x64/ARM64
shall fail compilation. Direct versioned APIs remain available on all targets.
Default headers and shipped x86 _stat/_fstat symbols retain their _stat64 ABI;
selected _stat64 also retains its 56-byte record and 64-bit fields.

Metadata queries reuse the existing status providers and mappings. File size
must be nonnegative and at most 2147483647; each timestamp must lie in the
inclusive signed 32-bit range. An unrepresentable value returns -1 with ERANGE.
Null result/path returns -1 with EINVAL; invalid descriptors report EBADF;
other provider failures retain their existing error vocabulary. A non-null
output is fully zeroed before query/conversion and remains zero on failure.
Success returns zero and preserves errno. Device, inode, mode and link fields
use the existing WCRT mappings, including volume serial and low inode bits;
uid/gid remain zero. No claim of historical VC6 device-number semantics is made.

## Verification and impact

TC-0086 checks every layout offset/type, caller packing, selected source use,
function pointers, guarded records, wrong-architecture rejection, native
path/descriptor equivalence and deterministic conversion/error boundaries.
Native/package x86/x64, ARM64 cross-links, TC-0049 regressions, source quality,
traceability, import audits and a selected Microsoft x86 DLL consumer apply.
[ADR-0008](adr-0008-explicit-stat32-source-route.md) records migration and ABI
preservation. New versioned symbols require opt-in/rebuild; existing symbols,
time_t/fpos_t, dependencies and Windows imports remain unchanged. The profile
is a bounded source route; licensed VC6 inventory and differential qualification
remain Unknown. Full legacy time/device/FILE ABI compatibility is separate work.
