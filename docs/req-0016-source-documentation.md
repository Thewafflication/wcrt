# REQ-0016 — Source documentation and line length

**Index:** [C89 Requirements](REQUIREMENTS.md)  
**Draft annotation:** Project quality requirement; not specified by C89

## Scope

This requirement applies to every project-owned C source file and header,
including implementation, platform, and test code. Generated files and
third-party code are excluded.

## Required files

- `Doxyfile` — Doxygen configuration for project-owned C sources and headers.
- `tests/c89/run-tc-0016.ps1` — TC-0016 test runner.

## Requirements

- Every project-owned `.c` and `.h` file shall contain a Doxygen-style file
  comment using `/** ... */` or `/*! ... */` syntax and an `@file` command.
- Every function declaration and definition shall have a Doxygen-style comment
  that describes its purpose, parameters, return value, and observable error or
  side-effect behavior where applicable.
- Public macros, constants, types, structures, unions, enumerations, objects,
  and structure members shall have Doxygen-style documentation.
- Internal functions, types, objects, and non-obvious implementation decisions
  shall have Doxygen-style documentation sufficient to maintain the code
  without relying on undocumented behavior.
- Documentation shall describe contracts and intent rather than merely repeat
  an identifier or restate the implementation.
- Every physical line in a project-owned `.c` or `.h` file shall contain no more
  than 80 characters, including indentation and comments.
- Source-quality checks shall report the file and line number for each
  violation and shall return a nonzero exit status when any violation exists.

## Acceptance

TC-0016 shall scan every in-scope `.c` and `.h` file, reject lines longer than
80 characters, run Doxygen with warnings treated as errors, and fail when a
required documented entity is missing or incompletely documented.

## Implementation record

- `Doxyfile` enables strict C extraction, static-entity extraction, complete
  parameter checks, and warnings-as-errors.
- `tests/c89/run-tc-0016.ps1` scans every project-owned C source and header,
  enforces the physical-line limit and `@file` comments, and runs Doxygen.
- The runner accepts Doxygen from `PATH` or the ignored portable tool location
  `build/tools/doxygen/doxygen.exe`.
