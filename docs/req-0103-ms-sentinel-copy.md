# REQ-0103: Microsoft bounded sentinel copying

string.h and a repeatable memory.h forwarding header shall declare
void *_memccpy(void *destination, const void *source, int sentinel, size_t count).
Copy bytes in order up to count, stopping after the first copied byte equal to
(unsigned char)sentinel. Return the address immediately after that byte, or
NULL if no match occurs. Preserve source contents, untouched destination bytes
and errno. Zero count returns NULL without accessing either buffer, including
NULL pointers. Nonzero counts require valid, sufficiently sized, disjoint
buffers; overlapping and invalid nonzero buffers are outside the contract.
No allocation, Windows API or modern invalid-parameter handler is introduced.

## Verification and impact

TC-0103 checks the function-pointer signature, repeated/reversed header order,
all byte values, negative/high sentinel conversion, each match position,
count limits, embedded zero, guards, source preservation and seeded errno.
Native/static/DLL x86/x64 and ARM64 compile/link evidence apply. Supplementary
Microsoft consumers call the shared checker at /Od and /O2 with no host CRT.
Update the manifest, aggregate, standalone-header and VC6 inventory records;
renew public-header hashes and run existing C89 string regressions, full suites,
quality, traceability and x86 imports. Work record contains estimates/review.
Genuine VC6/reference qualification remains Unknown.

[Microsoft reference](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/memccpy)
supports the signature and stop/return behavior. Unsigned sentinel conversion
and zero-count NULL inputs are explicitly selected WCRT behavior pending a
frozen reference inventory. Existing C89 memory functions are unchanged.
