# REQ-0077: Microsoft I64 integer format modifier

WCRT's narrow printf/scanf families shall recognize the Microsoft `I64`
length modifier for `d`, `i`, `u`, `o`, `x`, `X` and `n`. Signed arguments
and destinations are 64-bit signed integers; unsigned arguments and
destinations are 64-bit unsigned integers. TinyCC uses `long long` for these
types; VC6 uses `__int64` in its consumer declarations. This extension adds
no public symbol and is available with the existing Microsoft compatibility
surface in C89 and C99 modes.

The modifier appears after flags, width and precision for output, and after
suppression and width for input. Flags, width, precision, radix, assignment
suppression, conversion counts and retained stream lookahead shall behave as
the corresponding existing 64-bit integer path. `%I64n` stores the count in a
64-bit signed destination and retains WCRT's existing enabled count-output
policy; modern UCRT's disabling policy is not adopted. A following argument
shall be retrieved/stored independently with no vararg displacement error.
Incomplete output directives shall report failure without reading beyond the
format terminator. Invalid modifier/conversion combinations and input values
not representable in their destination remain outside the contract.

The C99 `ll` modifier and existing bounded-output contracts remain unchanged.
Wide Microsoft formatting semantics, `I32`, pointer-sized `I`, locale-object
variants, and historical floating-point spellings are not added by this slice.
The scanner's shared engine also accepts I64 for wide input as an additive
implementation behavior; this is not a claim of VC6-wide profile completion.

## Baseline, verification and impact

The selected VC6-era roadmap explicitly includes `%I64`. Microsoft's
[format reference](https://learn.microsoft.com/en-us/cpp/c-runtime-library/format-specification-syntax-printf-and-wprintf-functions)
and [scan reference](https://learn.microsoft.com/en-us/cpp/c-runtime-library/scanf-width-specification)
are supplementary contemporary references, not a substitute for genuine VC6
qualification. That compiler/ABI/differential gate remains Unknown.

TC-0077 covers all integer conversions, signed/unsigned boundaries, counts,
following arguments, width/precision, truncation, va-list forwarding,
suppression, match failure, and stream lookahead on x86/x64 and ARM64 where
available. TC-0029/0030 and TC-0020 cover ISO and legacy regression. The change
reuses the existing LL paths; no public layouts, imports, dependencies,
privileges, allocation policy or minimum Windows versions change.
