# C89 Requirements Index

WCRT assigns one independently numbered requirement document to each public
C89 header. Clause numbers annotate Clause 4 of the late ANSI X3J11 C89 draft:

<https://bohr.wlu.ca/hfan/cp264/references/c89-draft.html>

Any difference from ANSI X3.159-1989 or ISO/IEC 9899:1990 shall be resolved in
`docs/c89-deviations.md` before a conformance claim is made.

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

## Shared acceptance gates

Every requirement is complete only when its header and implementation files
exist; all named functions, macros, types, objects, and structure members are
tested; TinyCC builds do not silently use a host CRT; tests map back to the
cited draft clauses; Windows 2000 x86 import checks pass where applicable; and
implementation-defined choices and deviations are documented.

Shared runtime requirements—startup, ABI, Windows adaptation, imports, and
test-manifest structure—will receive subsequent requirement numbers rather than
being nested beneath a header requirement.
