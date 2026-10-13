# REQ-0088: Microsoft descriptor size and 64-bit position

io.h shall declare _lseeki64(int, wcrt_ms_int64, int), _telli64(int) and
_filelengthi64(int), all returning wcrt_ms_int64, and long _filelength(int).
_lseeki64 shall use the existing seek contract for SEEK_SET/CUR/END, signed
64-bit offsets and return positions. Successful repositioning clears the
associated stream EOF, pushback and conversion state; failure retains it.
_telli64 shall query the physical position without modifying stream state.

Length queries shall return the current physical size of a regular file,
without changing position or stream EOF/error/pushback/conversion state.
_filelength shall reject sizes above 2147483647 with -1/EFBIG. The wide
query accepts every nonnegative signed 64-bit size; negative metadata reports
-1/EIO. Nonregular handles report -1/ESPIPE. Invalid/closed descriptors report
-1/EBADF. Other seek/status errors retain existing backend mappings. Success
preserves errno. Queries are snapshots; concurrent file changes are not atomic.

## Verification and impact

TC-0088 covers function-pointer types, empty/text/binary sizes, positions beyond
4 GiB without extending files, seek origins/failures, shared duplicate position,
FILE state preservation, closed/invalid/pipe descriptors and injected metadata
boundaries/errors. Native/package x86/x64, ARM64 cross-links and selected
Microsoft consumers apply with TC-0043/0067/0079 regressions and shared gates.
The controlled io.h inventory grows by four functions; existing signatures,
stream/record layouts, Windows imports and dependencies remain unchanged.
No file mutation occurs during queries. Large-file allocation is excluded from
fixtures; deterministic providers test size limits. Scheduling and review are
recorded in the tranche work log. Genuine VC6 inventory/reference qualification
remains Unknown, including nonregular-handle historical semantics.
REQ-0092 supplies explicit WCRT_MS_ERRNO vocabulary for EIO/EFBIG to source
consumers that need these existing codes without enabling POSIX declarations.
