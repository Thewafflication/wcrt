# REQ-0014 — `<string.h>`

**Index:** [C89 Requirements](REQUIREMENTS.md)  
**Draft annotation:** §4.11 String handling

## Required files

- `include/string.h` — public declarations, `size_t`, and `NULL`.
- `src/string.c` or `src/string/` — byte and string operations.
- `tests/c89/string.c` — exhaustive boundary and overlap tests.

## Functions

| Draft clause | Required functions |
| --- | --- |
| §4.11.2 Copying | `memcpy`, `memmove`, `strcpy`, `strncpy` |
| §4.11.3 Concatenation | `strcat`, `strncat` |
| §4.11.4 Comparison | `memcmp`, `strcmp`, `strcoll`, `strncmp`, `strxfrm` |
| §4.11.5 Search | `memchr`, `strchr`, `strcspn`, `strpbrk`, `strrchr`, `strspn`, `strstr`, `strtok` |
| §4.11.6 Miscellaneous | `memset`, `strerror`, `strlen` |

## Requirements

- Byte comparisons shall use values interpreted as `unsigned char`.
- `memmove` shall handle every overlap direction; functions whose contracts
  prohibit overlap need not make overlapping use safe.
- Bounded string operations shall implement exact padding, termination, and
  maximum-count behavior, including a zero count.
- Search functions shall handle empty strings, null terminators, and not-found
  results exactly as specified.
- `strcoll` and `strxfrm` shall agree under the active `LC_COLLATE`; the `C`
  locale is mandatory.
- `strtok` shall maintain only the state and delimiter behavior permitted by
  C89 and shall document its non-reentrant nature.
- `strerror` shall provide a valid message for supported error values and the
  draft-permitted result for unknown values.

## Acceptance

Tests shall cover zero and one lengths, alignment variations, overlap, embedded
zero bytes, high-bit bytes, empty strings, terminators, padding, locale
collation, `strtok` sequences, side effects, and large-size boundaries feasible
on each target. Shared gates apply.

## C89 milestone design

The implementation is self-contained and uses unsigned bytes for memory and
lexicographic comparisons. `strcoll` and `strxfrm` implement the mandatory C
locale as bytewise collation. `strtok` uses one process-wide continuation
pointer and is intentionally non-reentrant. `strerror` provides stable static
messages for `EDOM`, `ERANGE`, and unknown values.
