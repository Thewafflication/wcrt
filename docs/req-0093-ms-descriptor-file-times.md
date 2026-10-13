# REQ-0093: Selected Microsoft descriptor file-time updates

sys/utime.h shall declare default-cdecl _futime32(int, struct __utimbuf32 *)
and _futime64(int, struct __utimbuf64 *). Existing explicit record layouts
remain unchanged. Both update access/modification times of the regular file
behind a live WCRT descriptor, using a borrowed native handle. They shall not
close/reopen it, move its position, flush data, change creation time or alter
FILE translation, EOF, error, pushback, orientation or buffering state.

Invalid descriptors/handles return -1/EBADF; valid pipe/character handles return
-1/EINVAL before timestamp conversion. Explicit timestamps and null-current
time use REQ-0048's checked ranges and conversions. Invalid ranges return
-1/EINVAL before update. Windows update denial returns -1/EACCES, with existing
REQ-0048 native error categories retained. Success returns zero and preserves
errno. The native handle must grant write-attributes access; no path reopen or
WCRT access-flag promotion is attempted.

Disk-handle metadata must identify a regular file: directory/other metadata
returns -1/EINVAL before clock conversion or update. Metadata-query failure
retains its existing error category. This prevents adopted directory handles
from bypassing the selected regular-file contract.

WCRT_VC6_UTIME32 before first header inclusion additionally aliases _futime to
_futime32 on x86, with the existing _utimbuf route. Without selection no generic
_futime macro or binary export is provided. Other targets retain explicit APIs
and existing selector rejection. Generic time_t remains 64-bit.

## Verification and impact

TC-0093 verifies declaration pointers, explicit/current timestamps, metadata
round trips, position/state and ownership preservation, duplicates, denied
native access, invalid/nonregular descriptors, range and provider failures.
Native/static/DLL x86/x64 and ARM64 compile/link evidence apply. The optional
Microsoft consumer exercises selected declaration/call ABI. REQ-0048/0091,
quality, traceability and Windows 2000 imports remain gates. Added symbols and
one source alias require rebuilt packages; no new Windows API, dependency,
existing layout or target change is intended. Work records cover estimates,
review and closeout. Genuine VC6/reference qualification remains Unknown.
