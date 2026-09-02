# Requirements Index

WCRT assigns independently numbered requirement documents to each public
standard-library conformance unit and to project-wide quality or platform
requirements. C89 clause numbers annotate Clause 4 of the late ANSI X3J11 C89
draft where applicable:

<https://bohr.wlu.ca/hfan/cp264/references/c89-draft.html>

Any difference from ANSI X3.159-1989 or ISO/IEC 9899:1990 shall be resolved in
`docs/c89-deviations.md` before a conformance claim is made.

Requirement records follow the pinned WSP requirement format. WCRT tailors
WSP's atomic-record convention by treating each public C89 conformance unit as
one stable requirement whose individually verifiable obligations are listed in
its singular **Requirement** section. See the [WSP adoption record](WSP-ADOPTION.md)
and [project test strategy](TEST-STRATEGY.md).

| Requirement | Test case | Header | Draft clause | Scope |
| --- | --- | --- | --- | --- |
| [REQ-0001](req-0001-assert.md) | [TC-0001](tc-0001-assert.tex) | `<assert.h>` | §4.2 | Diagnostics |
| [REQ-0002](req-0002-ctype.md) | [TC-0002](tc-0002-ctype.tex) | `<ctype.h>` | §4.3 | Character handling |
| [REQ-0003](req-0003-errno.md) | [TC-0003](tc-0003-errno.tex) | `<errno.h>` | §4.1.3 | Error reporting |
| [REQ-0004](req-0004-float.md) | [TC-0004](tc-0004-float.tex) | `<float.h>` | §4.1.4 | Floating-point limits |
| [REQ-0005](req-0005-limits.md) | [TC-0005](tc-0005-limits.tex) | `<limits.h>` | §4.1.4 | Integer limits |
| [REQ-0006](req-0006-locale.md) | [TC-0006](tc-0006-locale.tex) | `<locale.h>` | §4.4 | Localization |
| [REQ-0007](req-0007-math.md) | [TC-0007](tc-0007-math.tex) | `<math.h>` | §4.5 | Mathematics |
| [REQ-0008](req-0008-setjmp.md) | [TC-0008](tc-0008-setjmp.tex) | `<setjmp.h>` | §4.6 | Non-local jumps |
| [REQ-0009](req-0009-signal.md) | [TC-0009](tc-0009-signal.tex) | `<signal.h>` | §4.7 | Signals |
| [REQ-0010](req-0010-stdarg.md) | [TC-0010](tc-0010-stdarg.tex) | `<stdarg.h>` | §4.8 | Variable arguments |
| [REQ-0011](req-0011-stddef.md) | [TC-0011](tc-0011-stddef.tex) | `<stddef.h>` | §4.1.5 | Common definitions |
| [REQ-0012](req-0012-stdio.md) | [TC-0012](tc-0012-stdio.tex) | `<stdio.h>` | §4.9 | Input/output |
| [REQ-0013](req-0013-stdlib.md) | [TC-0013](tc-0013-stdlib.tex) | `<stdlib.h>` | §4.10 | General utilities |
| [REQ-0014](req-0014-string.md) | [TC-0014](tc-0014-string.tex) | `<string.h>` | §4.11 | String handling |
| [REQ-0015](req-0015-time.md) | [TC-0015](tc-0015-time.tex) | `<time.h>` | §4.12 | Date and time |
| [REQ-0016](req-0016-source-documentation.md) | [TC-0016](tc-0016-source-documentation.tex) | All project-owned C sources | N/A | Doxygen documentation and 80-column limit |
| [REQ-0017](req-0017-optional-console-startup.md) | [TC-0017](tc-0017-optional-console-startup.tex) | Static console startup | N/A | Optional WCRT-owned PE entry point |
| [REQ-0018](req-0018-optional-gui-startup.md) | [TC-0018](tc-0018-optional-gui-startup.tex) | Static GUI startup | N/A | Optional WCRT-owned `WinMain` PE entry point |

## C99 extensions

These requirements extend the C89 baseline. A proposed extension is not part
of WCRT's implemented conformance claim until its requirement and verification
record are accepted and its tests pass.

The dependency-ordered inventory and proposed identifiers for the remaining
C99 work are maintained in the
[C99 requirements backlog](C99-REQUIREMENTS.md). Planned identifiers do not
become controlled requirements, and do not enter the traceability baseline,
until their requirement and verification records are added.

| Requirement | Test case | Header | C99 clause | Scope |
| --- | --- | --- | --- | --- |
| [REQ-0019](req-0019-stdio-bounded-format.md) | [TC-0019](tc-0019-stdio-bounded-format.tex) | `<stdio.h>` | §7.19.6.5, §7.19.6.12 | `snprintf` and `vsnprintf` bounded formatted output |
| [REQ-0021](req-0021-stdbool.md) | [TC-0021](tc-0021-stdbool.tex) | `<stdbool.h>` | §6.2.5, §6.3.1.2, §7.16 | Boolean type and values |
| [REQ-0022](req-0022-stdint.md) | [TC-0022](tc-0022-stdint.tex) | `<stdint.h>` | §7.18 | Integer types, limits, and constants |
| [REQ-0023](req-0023-limits-long-long.md) | [TC-0023](tc-0023-limits-long-long.tex) | `<limits.h>` | §5.2.4.2.1, §7.10 | `long long` limits |
| [REQ-0024](req-0024-float-c99.md) | [TC-0024](tc-0024-float-c99.tex) | `<float.h>` | §5.2.4.2.2, §7.7 | Floating-point characteristics |
| [REQ-0025](req-0025-va-copy-restrict.md) | [TC-0025](tc-0025-va-copy-restrict.tex) | `<stdarg.h>`, public headers | §6.7.3, §7.15 | `va_copy` and `restrict` contracts |
| [REQ-0026](req-0026-iso646.md) | [TC-0026](tc-0026-iso646.tex) | `<iso646.h>` | §7.9 | Alternative operator spellings |
| [REQ-0027](req-0027-inttypes.md) | [TC-0027](tc-0027-inttypes.tex) | `<inttypes.h>` | §7.8 | Greatest-width integer interfaces |
| [REQ-0028](req-0028-stdlib-c99.md) | [TC-0028](tc-0028-stdlib-c99.tex) | `<stdlib.h>` | §7.20 | C99 general utilities |
| [REQ-0029](req-0029-stdio-c99-format.md) | [TC-0029](tc-0029-stdio-c99-format.tex) | `<stdio.h>` | §7.19.6.1 | C99 narrow formatted output |
| [REQ-0030](req-0030-stdio-c99-scan.md) | [TC-0030](tc-0030-stdio-c99-scan.tex) | `<stdio.h>` | §7.19.6.2, §7.19.6.9, §7.19.6.11, §7.19.6.14 | C99 narrow formatted input |
| [REQ-0031](req-0031-wchar.md) | [TC-0031](tc-0031-wchar.tex) | `<wchar.h>`, `<inttypes.h>` | §7.24, §7.8.2 | Wide I/O, conversion, strings, numeric text, and time |
| [REQ-0032](req-0032-wctype.md) | [TC-0032](tc-0032-wctype.tex) | `<wctype.h>` | §7.25 | Wide classification, descriptors, and mappings |
| [REQ-0033](req-0033-windows-utf16-abi.md) | [TC-0033](tc-0033-windows-utf16-abi.tex) | Wide types and streams | §7.17, §7.24.1, §7.24.6, §7.25.1 | Windows UTF-16 and wide-character ABI |
| [REQ-0035](req-0035-real-math.md) | [TC-0035](tc-0035-real-math.tex) | `<math.h>` | §7.12 | Complete C99 real mathematics and inquiry macros |
| [REQ-0036](req-0036-fenv.md) | [TC-0036](tc-0036-fenv.tex) | `<fenv.h>` | §7.6 | Flags, rounding control, and saved environments |
| [REQ-0037](req-0037-complex.md) | [TC-0037](tc-0037-complex.tex) | `<complex.h>` | §7.3 | Complex types, constants, accessors, and complete function families; selected TinyCC 1442 exact native Debug x86/x64/ARM64 Pass |
| [REQ-0038](req-0038-tgmath.md) | [TC-0038](tc-0038-tgmath.tex) | `<tgmath.h>` | §7.22 | Complete type-generic dispatch; selected TinyCC 1442 exact native Debug x86/x64/ARM64 Pass |
| [REQ-0039](req-0039-numeric-text.md) | [TC-0039](tc-0039-numeric-text.tex) | `<stdlib.h>`, `<stdio.h>` | §7.19.6, §7.20.1.3 | Narrow numeric text interchange allocation |
| [REQ-0040](req-0040-existing-header-audit.md) | [TC-0040](tc-0040-existing-header-audit.tex) | C89 header/runtime families | §7.1--§7.25 | Existing-header C99 integration audit |
| [REQ-0041](req-0041-c99-conformance-profile.md) | [TC-0041](tc-0041-c99-conformance-profile.tex) | All standard headers | §7.1--§7.25, Annex J | Clause-level conformance profile |
| [REQ-0042](req-0042-c99-compatibility-validation.md) | [TC-0042](tc-0042-c99-compatibility-validation.tex) | Whole product | §7.1.2, §7.1.4 | Header, ABI, regression, build, and target validation |

## Microsoft compatibility extensions

These nonstandard interfaces are isolated from the ISO conformance units so
that compatibility behavior cannot alter a standard function's contract.

| Requirement | Test case | Header | Source | Scope |
| --- | --- | --- | --- | --- |
| [REQ-0020](req-0020-ms-stdio-bounded-format.md) | [TC-0020](tc-0020-ms-stdio-bounded-format.tex) | `<stdio.h>` | Microsoft CRT | `_snprintf` and `_vsnprintf` legacy bounded output |
| [REQ-0043](req-0043-ms-io-header.md) | [TC-0043](tc-0043-ms-io-header.tex) | `<io.h>` | Microsoft CRT | Minimal low-level I/O header baseline |
| [REQ-0044](req-0044-ms-fileno.md) | [TC-0044](tc-0044-ms-fileno.tex) | `<stdio.h>` | Microsoft CRT | Stream-to-file-descriptor mapping |
| [REQ-0045](req-0045-ms-case-insensitive-string.md) | [TC-0045](tc-0045-ms-case-insensitive-string.tex) | `<string.h>` | Microsoft CRT | `_stricmp` and `_strnicmp` |
| [REQ-0046](req-0046-ms-secure-stdio.md) | [TC-0046](tc-0046-ms-secure-stdio.tex) | `<stdio.h>` | Microsoft secure CRT | `fopen_s` and `sscanf_s` |
| [REQ-0047](req-0047-ms-secure-string-copy.md) | [TC-0047](tc-0047-ms-secure-string-copy.tex) | `<string.h>` | Microsoft secure CRT | `strcpy_s`, `strncpy_s`, and `strcat_s` |
| [REQ-0048](req-0048-ms-utime.md) | [TC-0048](tc-0048-ms-utime.tex) | `<sys/utime.h>` | Microsoft CRT | `_utime32` and `_utime64` |
| [REQ-0049](req-0049-ms-stat.md) | [TC-0049](tc-0049-ms-stat.tex) | `<sys/stat.h>` | Microsoft CRT | `_stat64` and x86 `_stat` ABI alias |
| [REQ-0053](req-0053-ms-string-transform.md) | [TC-0053](tc-0053-ms-string-transform.tex) | `<string.h>` | Microsoft CRT | In-place case conversion and reversal |
| [REQ-0054](req-0054-ms-strdup.md) | [TC-0054](tc-0054-ms-strdup.tex) | `<string.h>` | Microsoft CRT | Allocating narrow-string duplication |
| [REQ-0055](req-0055-ms-string-set.md) | [TC-0055](tc-0055-ms-string-set.tex) | `<string.h>` | Microsoft CRT | In-place bounded character replacement |
| [REQ-0056](req-0056-ms-integer-string.md) | [TC-0056](tc-0056-ms-integer-string.tex) | `<stdlib.h>` | Microsoft CRT | Integer-to-string radix conversions |
| [REQ-0057](req-0057-ms-memory-case.md) | [TC-0057](tc-0057-ms-memory-case.tex) | `<string.h>` | Microsoft CRT | Case-insensitive memory comparison |

## POSIX compatibility extensions

These interfaces form a bounded Windows portability layer selected with
`WCRT_POSIX`. WCRT does not define `_POSIX_VERSION` or claim that the platform
conforms to POSIX.1-2017.

| Requirement | Test case | Header | Source | Scope |
| --- | --- | --- | --- | --- |
| [REQ-0050](req-0050-posix-selection-errno.md) | [TC-0050](tc-0050-posix-selection-errno.tex) | `<errno.h>`, `<time.h>` | POSIX and Microsoft/TinyCC ABI | Explicit selection, error names, strict isolation, and time-type interoperation |
| [REQ-0051](req-0051-posix-stat-utime.md) | [TC-0051](tc-0051-posix-stat-utime.tex) | `<sys/types.h>`, `<sys/stat.h>`, `<utime.h>` | POSIX.1-2017 | Typed `stat` and `utime` Windows adapters |
| [REQ-0052](req-0052-posix-dirent.md) | [TC-0052](tc-0052-posix-dirent.tex) | `<dirent.h>` | POSIX.1-2017 | Directory enumeration and stream lifetime adapters |

## Shared acceptance gates

Every requirement is complete only when its header and implementation files
exist; all named functions, macros, types, objects, and structure members are
tested; TinyCC builds do not silently use a host CRT; tests map back to the
cited draft clauses; Windows 2000 x86 import checks pass where applicable; and
implementation-defined choices and deviations are documented.

Shared runtime requirements—startup, ABI, Windows adaptation, imports, and
test-manifest structure—will receive subsequent requirement numbers rather than
being nested beneath a header requirement.

The rationale for keeping console and GUI startup outside `libwcrt.a` is
recorded in
[ADR-0001](adr-0001-separate-optional-pe-startup-objects.md).
