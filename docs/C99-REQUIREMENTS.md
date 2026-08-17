# C99 Requirements Backlog

**Content type:** Requirements planning record

**Status:** Controlled through REQ-0042; exact-candidate target verification
pending

**Source:** ISO/IEC 9899:1999 library clauses, as corrected through the C99
technical corrigenda

## Purpose

This record decomposes the C99 library milestone into reviewable conformance
units before implementation begins. It is not a conformance claim. An item
becomes part of the controlled baseline only when its `docs/req-NNNN-*.md`
record and corresponding verification specification are accepted.

C99 includes the C95 amendment. Consequently `iso646.h`, `wchar.h`, and
`wctype.h` are C99 prerequisites even though they did not originate in 1999.
C11 atomics and threads are deliberately excluded.

## Priority 1 — Fundamental types and macros

### Baselined REQ-0021 — C99 Boolean type and values

**Scope:** The C99-mode `stdbool.h` public header. C89-mode isolation is included
as a compatibility check; compiler implementation of the `_Bool` language type
is not supplied by the runtime.

**Requirement:** WCRT shall provide a self-contained `stdbool.h` that defines
`bool` as `_Bool`, `true` as the integer constant `1`, `false` as the integer
constant `0`, and `__bool_true_false_are_defined` as `1` in C99 mode. The
definitions shall be usable in integer constant expressions, repeated inclusion
shall be harmless, and the header shall not introduce `_Bool` syntax into the
C89 library surface.

**Verification:** TC-0021 compiles presence, value, constant-
expression, conversion, repeated-inclusion, and C89-isolation checks with each
supported TinyCC target.

**Dependencies:** TinyCC `_Bool` capability probe and the public-header edition
selection policy.

### Baselined REQ-0022 — C99 integer types, limits, and constants

**Scope:** The C99-mode `stdint.h` public header for x86, x64, and ARM64 Windows
ABIs. Optional exact-width typedefs are required whenever the target provides
the corresponding representation without padding bits.

**Requirement:** WCRT shall provide the exact-width, minimum-width, fastest
minimum-width, pointer-capable, and greatest-width signed and unsigned integer
typedefs required by C99 for the target ABI. It shall expose the associated
minimum and maximum macros, `SIZE_MAX`, `PTRDIFF_*`, `SIG_ATOMIC_*`, `WCHAR_*`,
and `WINT_*` macros when applicable, and the `INTN_C`, `UINTN_C`, `INTMAX_C`, and
`UINTMAX_C` constant-expression macros with types prescribed by C99. The header
shall document every optional typedef or macro omitted because the target lacks
a conforming representation.

**Verification:** TC-0022 verifies declaration presence, signedness,
sizes, limits, constant-expression usability, expression types, pointer round
trips, self-containment, C89 isolation, and ABI agreement on every supported
architecture.

**Dependencies:** The Windows/TinyCC data-model record, `stddef.h`, `limits.h`,
and verified `long long` compiler behavior.

## Priority 2 — Integer integration

REQ-0023 through REQ-0026 are baselined as the remainder of Milestone 2A.
REQ-0027 through REQ-0033 and REQ-0035 through REQ-0039 are implemented.
T6 removes the earlier `fma` implementation deviation, completes the remaining
REQ-0039 profile allocation, and controls REQ-0040 through REQ-0042. The
selected TinyCC 1442 package passes local x86/x64 native and ARM64 compile/link
verification. Exact-candidate native ARM64 verification and independent review
remain open; no historical or cross-target result is promoted to Pass.

| ID | Conformance unit | Primary C99 clauses | Principal obligations |
| --- | --- | --- | --- |
| REQ-0023 (baselined) | C99 `limits.h` additions | 5.2.4.2.1, 7.10 | `LLONG_MIN`, `LLONG_MAX`, and `ULLONG_MAX` with correct expression types |
| REQ-0024 (baselined) | C99 `float.h` additions | 5.2.4.2.2, 7.7 | `DECIMAL_DIG`, evaluation-method macros, and documented TinyCC floating model |
| REQ-0025 (baselined) | C99 variable arguments and declarations | 6.7.3, 7.15 | `va_copy` semantics plus edition-aware `restrict` contracts |
| REQ-0026 (baselined) | Alternative spellings | 7.9 | Complete, self-contained `iso646.h` macro set |
| REQ-0027 (baselined) | Greatest-width integers | 7.8 | `inttypes.h` types, functions, and all applicable print/scan macros |
| REQ-0028 (baselined) | C99 general utilities | 7.20 | `_Exit`, `atoll`, `llabs`, `lldiv`, and extended `strto*` conversions |

## Priority 3 — Input/output and character support

| ID | Conformance unit | Primary C99 clauses | Principal obligations |
| --- | --- | --- | --- |
| REQ-0029 (baselined) | C99 formatted output | 7.19.6.1 | New length modifiers and conversions; REQ-0019 remains independently controlled |
| REQ-0030 (baselined) | C99 formatted input | 7.19.6.2, 7.19.6.4, 7.19.6.7, 7.19.6.9, 7.19.6.11, 7.19.6.14 | New modifiers/conversions and the three `v*scanf` functions |
| REQ-0031 (implemented) | Wide-character I/O and strings | 7.24, 7.8.2 | Complete `wchar.h`, wide `inttypes.h` conversions, stream orientation, conversions, strings, time, and `mbstate_t` |
| REQ-0032 (implemented) | Wide-character classification | 7.25 | Complete `wctype.h` classification, descriptors, and mappings |
| REQ-0033 (implemented) | Windows UTF-16 ABI | 7.17, 7.24.1, 7.24.6, 7.25.1; implementation-defined ABI | `wchar_t`, promotion-safe `wint_t`, multibyte state, locale, stream state, and Windows interoperability by architecture |
| REQ-0034 | Unicode GUI startup | Project extension | Separately selected `wWinMain` startup after REQ-0031 through REQ-0033 |

## Priority 4 — Numeric environment and mathematics

| Planned ID | Conformance unit | Primary C99 clauses | Principal obligations |
| --- | --- | --- | --- |
| REQ-0035 (implemented; target review open) | C99 real mathematics | 7.12 | Classification/comparison macros and complete float, double, and long-double families |
| REQ-0036 (implemented; review open) | Floating-point environment | 7.6 | `fenv.h`, flags, rounding control, saved environments, and architecture profile |
| REQ-0037 (implemented; review open) | Complex arithmetic | 7.3 | `complex.h`, constants, accessors, and complete complex function families |
| REQ-0038 (implemented; review open) | Type-generic mathematics | 7.22 | Correct real/complex dispatch without multiple evaluation |
| REQ-0039 (implemented/profiled) | Numeric text interchange | 7.12, 7.19, 7.20 | Hexadecimal floating syntax, rounding, range errors, formatted I/O integration, and documented decimal accuracy boundary |

## Priority 5 — Conformance closure

| Planned ID | Conformance unit | Principal obligations |
| --- | --- | --- |
| REQ-0040 (implemented) | Existing-header C99 audit | Changed declarations, `restrict` contracts, macros, constraints, and behavior across the C89 baseline |
| REQ-0041 (implemented) | C99 conformance profile | Clause-level matrix of implemented, optional, implementation-defined, compiler-blocked, and deviating behavior |
| REQ-0042 (implemented; target evidence open) | C99 compatibility validation | Header self-containment, C89 isolation, ABI checks, and regression execution on x86, x64, and ARM64 |

## Sequencing rule

REQ-0021 through REQ-0026 form the baselined Milestone 2A. `inttypes.h`
depends on `stdint.h`; T3 implements REQ-0033 first, then REQ-0031 and
REQ-0032. `tgmath.h` depends on complete real and complex function families.
C11 threading work begins only after the C99 exit condition unless the roadmap
is explicitly re-baselined with its C99 dependencies and compatibility risks.
