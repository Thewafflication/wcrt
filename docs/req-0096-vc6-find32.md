# REQ-0096: Explicit narrow filename enumeration

io.h shall provide wcrt_findfirst32, wcrt_findnext32 and wcrt_findclose using
pointer-sized __wcrt_intptr_t handles and struct wcrt_finddata32: unsigned
attrib, signed long creation/access/write seconds, unsigned long size and
char name[260]. The record is 280 bytes with offsets 0/4/8/12/16/20.
Define _A_NORMAL/ _A_RDONLY/ _A_HIDDEN/ _A_SYSTEM/ _A_SUBDIR/ _A_ARCH as
0/1/2/4/16/32. Preserve caller packing around the record.

WCRT_VC6_FIND32 before first io.h inclusion selects x86 aliases _finddata_t,
_findfirst, _findnext and _findclose to these explicit interfaces. Reject
selection on other architectures. Default time_t and other records stay wide.

Use ANSI Windows wildcard matching and native order. Zero supplied output on
first failure; preserve the complete previous record on every next failure.
Null/empty specification, null output, zero/-1 handles,
invalid handles and unexpected Windows errors report EINVAL. Missing matches
or exhaustion report ENOENT; insufficient memory reports ENOMEM. Conversion
overflow reports ERANGE. Zero FILETIME means unavailable (-1); otherwise
signed seconds must fit long. File size must fit unsigned long. Only the
five nonzero legacy attribute bits are returned. Success preserves errno.

First returns -1 on failure or a caller-owned search handle on success. Next
returns 0/-1 and does not close its handle, even on exhaustion or conversion
failure; conversion failure advances enumeration. Close returns 0/-1.
First conversion failure closes its acquired handle and retains ERANGE/EINVAL
even if cleanup fails. No new host CRT dependency or Windows API import.
Track runtime-owned searches and reject unknown/closed handles before calling
Windows, whose FindClose can dereference an invalid opaque pointer. Successful
close removes ownership; failed close retains it for retry. Concurrent search
operations and stale handles after native handle reuse are outside this scope.

## Verification and impact

TC-0096 covers layout/source selection, guards, owned native search fixtures,
conversion/error providers, pointer widths and cleanup. Update build inputs,
test manifest and coverage inventory; rebuild supported packages. Existing
descriptor/time ABI is unchanged. Work record documents estimates/review.
Genuine VC6, reference qualification and generic legacy binary ABI remain open.

REQ-0100 corrects the original failed-next zero-output policy to documented
record retention, and adds _fsize_t spelling without changing the 280-byte
layout. TC-0096 native/provider and Microsoft checks verify unchanged failure
records; first/close ownership and error semantics remain intact.
