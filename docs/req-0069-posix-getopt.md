# REQ-0069 — Selected POSIX option parsing

**Content type:** Project requirement

**Status:** Implemented

**Source:** POSIX `getopt` and GNU-compatible `getopt_long`

## Scope

This requirement provides ordered short and long command-line parsing.
Argument permutation and `getopt_long_only` are excluded.

## Requirement

- `<getopt.h>` shall expose `optarg`, `optind`, `opterr`, `optopt`, `getopt`,
  `struct option`, long-argument constants, and `getopt_long` only when
  `WCRT_POSIX` is selected.
- Short parsing shall support clusters, attached and separate required
  arguments, GNU-style attached optional arguments, unknown options, leading
  colon missing-argument results, `--`, and stopping at the first operand.
- Long parsing shall support exact and unique abbreviated names, reject
  ambiguity, accept `=value`, consume separate required values, and implement
  `flag`/`val` results.
- Setting `optind` to zero or one shall reset internal cluster state.
- Parsing shall require no Windows API imports.

## Verification

**Method:** Selected/strict C89/C99 compilation and behavioral partitions

**References:** TC-0069

## Relationships

- **Depends on:** REQ-0050
- **Conflicts with:** Implicit argument permutation

## Implementation Record

`include/getopt.h` declares the selected source surface and `src/getopt.c`
implements ordered short and long parsing. TC-0069 covers clusters, arguments,
operands, reset, ambiguity, flag results, and isolation.
