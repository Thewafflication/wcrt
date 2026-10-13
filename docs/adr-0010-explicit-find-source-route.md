# ADR-0010: Explicit filename enumeration source route

Status: Accepted for the current compatibility work.

## Context

WCRT's default time_t is signed 64-bit, while selected VC6-era source records
need explicit signed-long timestamps. Filename metadata also has two distinct
size forms: unsigned-long and signed-64-bit. A generic record whose layout
implicitly follows the current time_t would not establish the intended narrow
source contract. Genuine VC6/service-pack and reference qualification remain
Unknown, so source aliases must not imply a frozen legacy binary profile.

## Decision

Expose versioned wcrt_finddata32/wcrt_finddata32i64 records and corresponding
first/next functions, with one owned-search close function. Declare fixed field
types and pack-8 layouts, restoring caller packing. The x86-only opt-in
WCRT_VC6_FIND32 maps generic/i64 source spellings to these interfaces; default
aliases remain absent. _fsize_t is independently unsigned long. Consumers using
generic calendar functions with the record's timestamps can additionally select
WCRT_VC6_TIME32 before first inclusion; enumeration records never silently
follow default time_t. Existing generic calendar/metadata binary ABI stays wide.

Track runtime-owned native searches to reject unknown/closed handles before
calling opaque Windows APIs. First failures zero output and release newly
acquired resources where possible; next failures retain the previous record
and keep ownership. Conversion uses temporary complete records with checked
time/size bounds. A successful native advance followed by conversion failure
still advances the search. ANSI matching and native ordering are inherited.

## Consequences and verification

Source code needs explicit profile selection, while all supported targets can
call the versioned APIs. Two record sizes and function pairs are additive
package exports. Registry concurrency and native handle reuse remain outside
the selected qualification scope. TC-0096/0100 verify layout/packing, aliases,
mixed records, boundaries, retained output, cleanup and native pattern/output
aliasing; supplementary Microsoft /Od-/O2 validates selected x86 declarations.
No claim of generic legacy binary equivalence, Unicode matching or licensed
VC6/reference qualification is made. Revisit those after freezing that inventory.
