# REQ-0079: VC6 x86 C consumer header foundation

WCRT shall provide a compiler-selected C header branch for Visual C++ 6.0
x86 consumers (`_MSC_VER <= 1200` and `_M_IX86`). The existing TinyCC C89/C99
branch remains the runtime build model. This is a WCRT DLL C-consumer path;
it does not claim that existing WCRT `time_t`, FILE, stat or startup objects
match every legacy MSVCRT layout or that WCRT itself builds with VC6.

The selected `<stddef.h>` branch shall supply unsigned 32-bit size_t,
signed 32-bit ptrdiff_t, unsigned 16-bit wchar_t and constant-expression
offsetof without compiler builtin names. `<stdarg.h>` shall provide a
char-pointer stack cursor with four-byte slot rounding for the default
x86 C calling convention, honoring scalar promotions and by-value aggregates.
va_end invalidates that cursor. C99 va_copy is not exposed by the VC6 compiler.
No x64, ARM64, C++ or nondefault calling convention is implied by this branch.

Unconditional 64-bit types in stdio/time and Microsoft explicit-width file
headers shall use the compiler-selected Microsoft integer typedefs. Their
existing widths, signedness, offsets and structure packing shall remain
unchanged: time_t and fpos_t remain signed 64-bit and `_stat64` remains 56
bytes on the x86 profile. These are WCRT types, not VC6's legacy 32-bit time_t
or `_stat` layouts. A distinct future selector is required for those ABIs.

`WCRT_DLL=1` shall mark the standard stream objects as imported data, matching
the existing errno import selector. Ordinary functions use the default C
calling convention; consumers shall compile with its cdecl default. Runtime
builds and static consumers leave WCRT_DLL undefined. The stream identities
and existing runtime data symbols remain unchanged.

## Verification and impact

TC-0079 checks the selected C89/MS headers independently and repeatedly with
no host header search. An optional supplied Microsoft compiler/librarian/linker
builds the x86 mixed-vararg consumer and Microsoft import library from the
runtime's definition, links with no default libraries, executes against WCRT,
and checks that the PE has no Microsoft CRT import.
Each optional Microsoft consumer runs under /Od and /O2; selected macro and
system-type fixtures participate in both configurations.
The supplied-tool run shall retain a JSON evidence record with full compiler
version, tool-entry hashes, compiler options, input/source/header/runtime hashes,
per-configuration executable hashes, execution and import results, and separate
compiler/reference qualification status. Inputs/tools must remain unchanged
through compilation/execution. A TinyCC-only run shall not replace that optional
Microsoft evidence with an unsupplied-compiler result.
The explicit test entry supplies the link-only Microsoft `_fltused` marker
for consumer floating-point instructions; it adds no runtime dependency.
The runner distinguishes genuine compiler version 12 qualification from a
supplementary newer-MSVC test of
the selected header branch. Missing genuine compiler evidence remains Unknown.
The genuine service-pack and reference-runtime identity is an additional
release qualification gate; this test alone does not close the full VC6 ABI
or differential profile.

REQ-0010/0011 compiler selection and REQ-0012 DLL data linkage are extended.
REQ-0015/0048/0049 types retain their existing ABI. Tests cover selected headers,
offsets, widths, promoted scalars, pointer/aggregate/64-bit varargs and va-list
forwarding to WCRT. Existing ISO ABI/header tests remain regression gates.
C89 call fixtures add small-structure div/ldiv returns, floating call/return
and output parameters, numeric parse end pointers, sorting/search callbacks
and allocation operations. Volatile function pointers force real calls for
interfaces the compiler might otherwise replace with intrinsics.
Macro fixtures additionally expand assertion re-inclusion, offsetof, integer
and floating model/limit constants, signal sentinels, setjmp and locale/ctype
interfaces. A binary32 representation check guards compiler literal spelling;
the setjmp fixture uses a valid controlling expression and volatile state.
The optional consumer also calls REQ-0076/0078/0080–0085 interfaces across
the DLL boundary: path helpers, 64-bit conversions, directory change/removal,
shared stream/descriptor opens, mode switching and drive queries.
Owned temporary files and
directories are removed, and the working directory is restored.
An additional translation unit selects REQ-0086's stat32 source route and
checks selected structure tags/function pointers, guarded records and
unchanged explicit _stat64 calls across the Microsoft x86 DLL boundary.
Another selected unit composes REQ-0091's narrow file-time source route with
stat32 and preserved explicit wide calls. Added descriptor size/EOF interfaces
exercise successful wide calls; native-handle adoption's Microsoft call sites
cover invalid handle/flag paths while its actual ownership transfer is verified
by the separate native/package TC-0090 fixtures.
No new runtime symbols, Windows imports, dependencies, privileges, allocation,
or OS baseline are introduced. Import-library generation is a verification
artifact; public release packaging is not changed by this tranche.
