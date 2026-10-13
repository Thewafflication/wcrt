# REQ-0089: Microsoft physical descriptor EOF query

io.h shall declare int _eof(int descriptor). For a regular file it shall
return one if the current physical position exactly equals current physical
size, and zero otherwise, including positions beyond the end. It shall return
-1 when size or position cannot be queried, retaining REQ-0088 error mappings:
invalid/closed descriptors EBADF, nonregular handles ESPIPE, negative provider
sizes EIO and existing provider/seek errors. Success preserves errno.

_eof shall not read bytes, seek to the end, clear or inspect FILE EOF as the
answer, change shared descriptor position or discard pushback/conversion/error
state. Text mode uses physical file bytes, including CRLF translation. Size
and position are snapshots; concurrent external mutation requires coordination.
DOS Ctrl-Z, _fmode, pipe/terminal historical behavior, genuine VC6 inventory
and differential reference qualification remain outside this selected contract.

## Verification and impact

TC-0089 checks exact-end/before-end/beyond-end and empty files, independent
FILE EOF state, pending pushback/error preservation, duplicated positions,
text physical size, invalid/closed/pipe handles and injected query failures.
Native/static/DLL x86/x64, ARM64 cross-links and supplementary Microsoft
consumer evidence apply with header, seek, C89, quality and traceability gates.
The controlled io.h inventory gains one runtime symbol. Existing stream/record
layouts, signatures, imports, dependencies and supported targets are unchanged.
Queries do not modify contents. Review/schedule/effort are recorded in the work
log; reference qualification remains Unknown.
