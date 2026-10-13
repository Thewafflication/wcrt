# Explicit stat32 source route tranche

## Plan, estimate and design

REQ-0086 addresses the roadmap's _stat migration obligation. Estimate: 100-150
implementation/header lines and 180-250 test lines. Dependencies: shared status
providers, descriptor-handle lookup, existing x86 aliases and consumer tooling.
Use explicit versioned symbols and an x86-only selector; preserve all shipped
aliases. Validate every narrow field that carries size/time before copying,
and keep output zero on failure. ADR-0008 records the source/ABI decision.

## Verification plan

Check all offsets, packing restoration, selected tag/function-pointer use,
wrong-target selection, guarded outputs, native file/descriptor agreement,
deterministic size/time extrema and provider errors. Verify the selector through
actual Microsoft x86 compiler call sites and the generated import library.
Require native/package x86/x64, ARM64 cross-links, TC-0049 regression, source
quality/traceability and Windows 2000 imports. Full VC6 device/time semantics
and genuine reference qualification remain explicit gaps.

## Closeout

On 2026-10-01 TC-0086 passed native x86/x64 behavior and deterministic provider
partitions, including fully zero output on conversion failures. The x86
selector passed guarded tag/function-pointer tests and static/DLL consumers;
x64 and ARM64 rejected the selector with the required diagnostic. Direct
versioned APIs passed x64 static/DLL consumers and ARM64 cross-links. ARM64
native execution remains Unknown. The supplementary Microsoft compiler major
16 x86 DLL consumer passed with a separately selected translation unit.
The import definition retains _stat/_fstat/_stat64/_fstat64 and adds distinct
wcrt_stat32/wcrt_fstat32 entries; TC-0049 passed on x86/x64.

Source quality passed for 316 files, traceability passed for 85 requirements,
and Windows 2000 imports passed without new APIs. Review enlarged trailing
guards so a mistaken 56-byte write into a 36-byte record is safely detected,
checked every offset and caller packing restoration, and tested both path
and handle conversion/error paths. Source-quality checks found four overlong
test comments; they were shortened before the final Pass. No production
failure was found. Header/implementation size remained within estimate;
tests exceeded the initial estimate after adding separate guarded selected
and Microsoft translation units. Elapsed effort was not reliably measured.
Genuine VC6 layout/runtime qualification, historical device semantics and
legacy time_t remain open.
