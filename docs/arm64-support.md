# Windows ARM64 support

WCRT supports building the C89 and C99 library profiles for Windows ARM64 with
TinyCC. Windows ARM64 belongs to the Windows 10 and Windows 11 compatibility
tier; it is not part of the Windows 2000 legacy tier.

## Current verification

The workflow assigns the ARM64 build job to the native `windows-11-arm`
runner. It runs the same C89 and C99 aggregates, capability probes, Debug
consumers, startup objects, and package inputs used for x86/x64. Exact source
`aea690d2caf108054d1644eefb176efde779f524` passes 15/15 C89, 30/30 combined
C99/compatibility, consumers, and startup in run `32027269426`.

On an x64 development host, `tools/run-c89-arm64-cross.ps1` and the focused
C99 runners use the AArch64 Windows TinyCC toolchain. Each generated binary is
parsed as PE/COFF and must carry machine type `0xAA64`; execution is reported
as deferred. Such compile/link evidence is useful, but it is not a native
behavior result and cannot satisfy an exact-candidate ARM64 release gate.

Evidence is written to:

```text
output/test-results/c89-wcrt-arm64/
output/test-results/arm64/
```

The ARM64 `setjmp` implementation preserves the Windows ABI nonvolatile
integer registers X19-X28, frame pointer X29, stack pointer, return address
X30, and the low 64-bit halves D8-D15 of vector registers V8-V15. The public
ARM64 `jmp_buf` size is compile-time checked as 168 bytes.

The retained TinyCC 1442 baseline required an ARM64 complex-helper register
adapter. TinyCC 1444 corrected its private helper declarations to the standard
Windows ARM64 function ABI: four binary64 components in `d0`--`d3` and the
result address in `x0`. WCRT continues to ship the target-scoped helper symbols
in `libwcrt.a` and `libwcrt-tinycc-complex-abi.a`, but their assembly entries
now preserve the correct registers and tail-call the scaled scalar helpers.
This compiler-private adaptation does not alter the public complex function
ABI. TC-0037 and the static/DLL consumers are the required native gate.

## Candidate evidence boundary

The exact Debug candidate evidence covers the full C89/C99/compatibility
aggregates, complex and floating ABI behavior, nonvolatile integer and floating
register preservation around `setjmp` and `longjmp`, default signal
termination, assertion diagnostics, static and DLL consumers, and both
optional startup objects. Tagged native ARM64 Release execution is still absent
and remains an R1 prepublication Unknown; local compile/link does not replace
it. It is not a T6 conformance failure because the complete native Debug matrix
passes, but publication still requires the optimized Release consumer/startup
smoke job. Windows 2000 import enforcement applies only to x86.
