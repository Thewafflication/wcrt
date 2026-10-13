# REQ-0095: Explicit narrow calendar-time source route

time.h shall declare default-cdecl wcrt_time32(long *), wcrt_mktime32(struct tm
*), wcrt_difftime32(long, long), wcrt_gmtime32(const long *), wcrt_localtime32
(const long *) and wcrt_ctime32(const long *). Long is signed 32-bit on all
supported Windows architectures. Calendar inputs/results range from Unix epoch
zero through 2147483647; numeric difference accepts all signed-long operands
and subtracts converted doubles without integer overflow. Success preserves
errno. Invalid/null calendar inputs and unrepresentable narrow results return
NULL or -1/EINVAL as appropriate. wcrt_time32 writes its result, including -1,
into a nonnull supplied destination without writing adjacent bytes.

wcrt_mktime32 shall normalize only a copied input, check local arithmetic and
the converted UTC narrow result, and commit normalized caller fields only on
success. Failed conversion leaves the input unchanged. Conversion/text results
reuse the existing REQ-0015 shared storage and Windows local-time policy;
tm_isdst remains indeterminate. Signed pre-epoch local seconds shall be
reconstructed correctly from FILETIME, permitting local/UTC epoch round trips.

WCRT_VC6_TIME32 before first time.h inclusion shall select x86 time_t as long
and object-like aliases time/mktime/difftime/gmtime/localtime/ctime to the
explicit narrow adapters. Reject selection on x64/ARM64 or with WCRT_POSIX,
whose timestamp-dependent binary layouts remain wide. Default time_t and
existing generic binary symbols remain signed 64-bit. Preserve clock_t,
struct tm, fpos_t, explicit status/time records and other time functions.
Compose the narrow source route with the existing stat32/utime32 selectors;
the runtime itself is built with the default wide time_t.

## Verification and impact

TC-0095 verifies layouts/signatures, guarded four-byte objects, source function
pointers, repeated/include-order headers, strict/POSIX/default isolation,
wrong-target/profile rejection, shared results, calendar boundaries, numeric
differences, normalization/no-mutation, current-time failure and local round
trips. Native/package x86 source selection, explicit x64 adapters, ARM64
cross-links and supplementary Microsoft consumers apply. Default wide C89/time,
POSIX time and source-file-time regressions, quality, traceability and Windows
2000 imports remain gates. Additive adapter symbols and a source selector need
rebuilt packages; existing layouts/symbols/dependencies/imports remain intact.
ADR-0009 and the work record cover design/risk/estimates. Genuine VC6 compiler,
service-pack/reference and complete timezone/error behavior remain Unknown.
