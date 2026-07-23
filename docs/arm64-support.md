# Windows ARM64 support

WCRT supports cross-compiling the implemented C89 requirements for Windows
ARM64 with TinyCC. Windows ARM64 belongs to the Windows 10 and Windows 11
compatibility tier; it is not part of the Windows 2000 legacy tier.

## Current verification

`tools/run-c89-arm64-cross.ps1` builds TC-0001 through TC-0011 with the
AArch64 Windows TinyCC toolchain. Each output is parsed as PE/COFF and must
carry machine type `0xAA64`. Runtime execution is deliberately reported as
`DeferredToArm64CI` until the tests run on a native or supported emulated ARM64
GitHub runner.

Evidence is written to:

```text
output/test-results/c89-wcrt-arm64/
```

The ARM64 `setjmp` implementation preserves the Windows ABI nonvolatile
integer registers X19-X28, frame pointer X29, stack pointer, return address
X30, and the low 64-bit halves D8-D15 of vector registers V8-V15. The public
ARM64 `jmp_buf` size is compile-time checked as 168 bytes.

## Deferred runtime gates

ARM64 CI shall execute the same behavioral sources used by x64. It shall also
exercise nonvolatile integer and floating register preservation around
`setjmp` and `longjmp`, default signal termination, assertion diagnostics, and
all Windows imports. Cross-build success alone is not a runtime-conformance
claim.
