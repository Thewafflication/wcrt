# ADR-0008: Explicit 32-bit file-status source route

## Status

Implemented decision for REQ-0086; genuine VC6 qualification remains Unknown.

## Context

REQ-0049's shipped x86 _stat/_fstat symbols accept the 56-byte WCRT _stat64
layout. Replacing their parameters with a 36-byte record would corrupt
existing consumers. The roadmap requires a separately selected legacy-width
source route and migration evidence. Exact VC6 reference semantics remain
unfrozen; current WCRT metadata mappings also differ from the historical
drive-number profile.

## Decision

Add versioned WCRT C interfaces `wcrt_stat32` and `wcrt_fstat32` using
`struct wcrt_stat32`, with signed 32-bit size/timestamps. Keep the original
symbols and _stat64 unchanged. The explicit x86-only `WCRT_VC6_STAT32=1`
header selector maps both the _stat tag/function spelling to wcrt_stat32
and _fstat to wcrt_fstat32. Object-like macros preserve function-address and
structure-tag source use consistently. A translation unit selects the route
before its first sys/stat.h inclusion. Other architectures reject the legacy
selector while retaining the explicit versioned interfaces.

Use distinct WCRT names to avoid TinyCC's existing Microsoft-stat linker
canonicalization and make the ABI choice visible in the import/export table.
The selected source route is not a binary replacement for historical MSVCRT.
WCRT time_t and fpos_t remain 64-bit, and the existing volume-serial/inode
mapping is retained. A full VC6 time/device profile remains separate work.
Reject unrepresentable size/timestamps with ERANGE before populating any
field; zero the supplied output on failure rather than partially narrowing it.

## Consequences and verification

Callers opt into the new layout and rebuild; default source and binary
consumers retain their contracts. Source records explicitly using _stat64
remain stable in selected translation units. Test the 36-byte layout and
every offset, restoration of caller packing, selected function pointers and
structure tags, wrong-architecture rejection, narrowing boundaries, guarded
records, native/package behavior and actual Microsoft x86 DLL calls. Preserve
the existing TC-0049 alias tests and the Windows 2000 import baseline. Genuine
VC6 compiler/service-pack/reference qualification remains a release gate.
