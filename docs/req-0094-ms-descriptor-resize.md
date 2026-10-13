# REQ-0094: Selected Microsoft descriptor resize

io.h shall declare _chsize(int descriptor, long size), with signed 32-bit
requested length on all supported Windows targets. A live writable regular
file shall be truncated or extended to the requested physical byte length.
Extension shall append zero bytes and preserve existing bytes. Translation
and append flags shall not change these raw resize operations.

Success returns zero, preserves errno and restores the original signed 64-bit
physical position, including positions beyond the new end or 32-bit range.
FILE EOF, error, pushback, orientation and buffering state remain unchanged;
duplicate descriptors observe the shared resized file and restored position.
Invalid descriptors return -1/EBADF; negative size returns -1/EINVAL after
descriptor validation. WCRT read-only access returns -1/EACCES even if the
underlying native handle has greater rights. Nonregular metadata returns
-1/EINVAL. Existing metadata/seek/native failure categories are retained.
Disk-full/zero-progress extension returns -1/ENOSPC; native denied resize
returns -1/EACCES. No operation closes or reopens the borrowed handle.

After beginning mutation, the adapter attempts to restore position on failure.
The first mutation error takes precedence over a subsequent restoration error.
If mutation succeeds but restoration fails, return -1 with the restoration
error. Failed resize may leave a partial size change; failed restoration can
leave the physical position changed. No concurrent shared-descriptor or atomic
resize guarantee is provided. Existing data/state must not be silently reset
to make a failed operation appear successful.

## Verification and impact

TC-0094 shall verify signatures, zero-filled extension, truncation, zero/same
length, append/text modes, duplicate/state/position preservation, negative and
invalid/nonregular/read-only inputs, partial writes, disk-full/native failures
and error precedence during restoration. Only small owned native files are
allocated; boundary/huge-file cases use providers. Native/static/DLL x86/x64,
ARM64 cross-links, supplementary Microsoft calls, TC-0067/0088 regressions,
quality, traceability and Windows 2000 imports apply. This adds one public
symbol, private resize adapter and reviewed baseline SetEndOfFile import;
no existing layout, generic alias, secure/wide variant or dependency changes.
The work record contains scope/risk/estimates and closeout. Microsoft's current
reference informs selected behavior:
https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/chsize
Genuine VC6 membership/compiler/reference qualification remains Unknown.
