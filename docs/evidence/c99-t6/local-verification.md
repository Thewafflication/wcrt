# C99 T6 Verification Record

**Content type:** Retained local and exact-revision CI verification evidence

**Candidate source:** `aea690d2caf108054d1644eefb176efde779f524`

**Execution date:** 2026-08-17 (America/Chicago)

**Local host:** Microsoft Windows 10.0.26200, x64 OS, x64 process,
PowerShell 7.6.3

**Exact CI:** GitHub Actions run `32027269426`, conclusion `success`

**Latest-dependency successor CI:** GitHub Actions run `32533622277` at
`eb27920bf5ceaa28874a78e05a3efb39da6761fc`, conclusion `success`; exact
machine-readable identities are in `latest-tinycc-provenance.json`

**Decision:** Exact-revision native Debug conformance matrix Pass; T6
completion Reject because independent review is incomplete; R1 readiness
Reject because WPM trust, installation, and the tagged optimized Release
matrix remain incomplete

**Scope reconciliation:** On 2026-08-21 the maintainer deferred Authenticode
and Defender from WCRT 1.0.0. The observed `NotSigned` and scan rows below are
retained historical candidate evidence and are not rewritten as Pass. WPM
signing/verification remains an active release gate.

## Dependency Baseline

The machine-readable dependency record is
`docs/evidence/c99-t6/dependency-provenance.json`. This is retained historical
T6 evidence, not a floating claim about the current repository index. The
selected WSP gitlink is `2198ccab08f969a789448767fe7017b774369adc`; TinyCC is
`0.9.28-rc.1442+2474e1c2` at source revision
`d5c02f0fcdfdf75265d38df6ff9db2f8067367ac`; WPM is 1.0.16; and cv2pdb is
0.54.0 at source revision `25cfecaad700550a49cd7ddbf7773254ea438d07`.
The exact release archives, installed executables, release keys, action SHAs,
runner images, jobs, and artifact identifiers are recorded there.
Successor workflows intentionally resolve the latest eligible `tinycc` WPM
package and retain the newly selected identity per target.

Exact CI used runner 2.336.0 and provisioner 20260729.566. x86/x64 used
`windows-2025-vs2026` image `20260810.198.2`; ARM64 used
`windows-11-arm64` image `20260809.134.1`.

## Gate Results

| Gate | x86 | x64 | ARM64 | Evidence and boundary |
| --- | --- | --- | --- | --- |
| TinyCC language/data model | Pass | Pass | Pass | Ten probes per target: seven Supported and three controlled compiler-blocked ExpectedFail pragmas; no unexpected failure |
| Header self-containment/repetition/mixed order | Pass | Pass | Pass | TC-0042; all 24 C99 headers, all 15 C89 headers, two C99 mixed orders, and one C89 mixed order |
| C89 TC-0001--TC-0015 | 15/15 Pass native | 15/15 Pass native | 15/15 Pass native | Exact-revision architecture artifacts from run `32027269426` |
| C99 TC-0019, 0021--0033, 0035--0042 | 22/22 Pass native | 22/22 Pass native | 22/22 Pass native | Exact-revision extension aggregates |
| Microsoft compatibility TC-0020, 0043--0049 | 8/8 Pass native | 8/8 Pass native | 8/8 Pass native | Exact-revision extension aggregates; 30/30 combined cases per target |
| C89 isolation and constant-expression/type checks | Pass | Pass | Pass | TC-0040/TC-0042 presence, declaration, expression-type, layout, and ABI cases |
| Clause/facility inventory | Pass | Pass | Pass | 25 clauses, 24 headers, 75 classified facility rows; profile validator Pass |
| WSP common tools | Pass | Pass | N/A host gate | Syntax, logging, traceability/evidence negative tests, reports, source quality, warning, checksum, and documentation negative tests |
| Traceability | Pass | Pass | Pass inventory | 48 requirements, 48 test specifications, 48 implementations |
| Source quality | Pass | Pass | Pass source | TC-0016: 182 C/header files, zero violations, Doxygen 1.17.0 |
| Debug build | Pass | Pass | Pass | Native CI DLL/static/startup builds and retained build artifacts |
| Debug static/DLL consumers | Pass | Pass | Pass | C89 and complex consumers execute natively; ARM64 DLL path proves public exports via `LoadLibraryA`/`GetProcAddress` and links only the private helper companion |
| Debug console/GUI startup | Pass | Pass | Pass | TC-0017/TC-0018 execute natively; no host CRT import |
| Release build | Pass | Pass | Pass compile/link | Local rebuild with executable hashes matching the exact CI dependency record |
| Release static/DLL consumers | Pass native | Pass native | Pass compile/link; native Unknown | Local readiness input only; tagged native Release evidence is required before publication and is not inferred from Debug or link evidence |
| Release console/GUI startup | Pass native | Pass native | Compile/link Pass; native Unknown | Local readiness input only; ARM64 outputs are PE `0xAA64` and the tagged optimized Release matrix has not run |
| Windows 2000 imports | Pass | N/A | N/A | x86 Release DLL `ddd4051a...`, machine `0x014C`, 27 allowlisted `kernel32.dll` imports, zero unexpected |
| Automatic built-in failure diagnostics | Pass | Pass | Pass | TinyCC crash trace identifies functions on all targets |
| Independent GDB backtrace | Pass | Pass | Unknown | GDB 17.1 Completed on x86/x64; ARM64 GDB 16.2 is `UnsupportedTarget`, not Pass |
| Package structure | Pass | Pass | Pass | One exact unsigned multi-architecture archive; required entries and complex profiles checked by `build-wpm-package.ps1` |
| WPM signature verification | Fail | Fail | Fail | WPM 1.0.16 exits 1: `unsigned packages are not allowed` |
| Authenticode/timestamp | Fail | Fail | Fail | Each Release DLL is `NotSigned`; no signer or timestamp exists |
| Defender release gate | Unknown | Unknown | Unknown | One local scan found no new detections in unsigned candidate directories, but is informational only; further design is deferred and final signed bytes do not exist |
| Installation/removal/rollback | Unknown | Unknown | Unknown | Not executed because it changes Program Files/registry and no separate authority was given |
| Exact-revision native CI | Pass | Pass | Pass | Run `32027269426` succeeded and retained per-target detailed results, build artifacts, and dependency provenance |

## Build and Artifact Identities

| Architecture | Configuration and origin | DLL SHA-256 | Length |
| --- | --- | --- | ---: |
| x86 | Debug, exact CI | `53adb2e934a5584be275288ec61bd0845f96fa7a6c36f99b16979bb8ed0e8e51` | 623212 |
| x64 | Debug, exact CI | `1bcbf15cc61d341ed6a602794b99b21408954fd61eb7a45d09eb8613c62a22d6` | 669804 |
| ARM64 | Debug, exact CI | `eafcb442cd243e10583677630420a0e8cba56a45fc8aed02713be86d8b578098` | 689772 |
| x86 | Release, local exact dependency | `ddd4051a236e9c278ba1a3833e0f60d362dcf5218b79c0e45a85dac5e0d5791b` | 150528 |
| x64 | Release, local exact dependency | `af0784a7570aa3c472ed796d2e28a6801a8c56e823f15250d41c357651c27511` | 170496 |
| ARM64 | Release, local exact dependency | `a43dae26b787476429c3a7a1931003f14436a810816e89ccacb51b5195ffcb8b` | 196096 |

All Release DLLs report file/product version
`1.0.0-rc.1.23+aea690d2`. The exact unsigned package is
`wcrt-any-1.0.0-rc.1.23+aea690d2.zip`, 418316 bytes, SHA-256
`18be4aa5086d07ed9ff31e4f808b107d14c399408e394877c57e9f51f288e68b`.
The package member and checksum-file identities are in
`docs/evidence/c99-t6/release-candidate-manifest.json`.

The exact CI Debug package is
`wcrt-debug-any-0.0.0-dev.23+aea690d2.zip`, 1555555 bytes, SHA-256
`a94c6bda2289623cf03c798b41a21bf15789e802a2cdbfd2400574d3d01438f5`.
It is verification evidence, not the Release candidate.

## Exact CI Evidence

GitHub Actions run `32027269426` at the candidate revision concluded
successfully. The retained jobs are source quality `95379192945`, x86
`95379408246`, ARM64 `95379408254`, x64 `95379408264`, and package
`95379833779`. Artifact identifiers are:

- `wcrt-x86-debug` `9287578726` and build artifact `9287579859`;
- `wcrt-x64-debug` `9287579186` and build artifact `9287580348`;
- `wcrt-arm64-debug` `9287591165` and build artifact `9287592047`;
- `wcrt-debug-any-package` `9287613328`.

Each architecture artifact retains `dependency-provenance.json`. The ARM64
named evidence artifact also retains `libwcrt-tinycc-complex-abi.a` (19600
bytes, SHA-256
`f637597ea9e38b8ff7afb85a77f94346a789e90eab56b506680e6039eb52cb71`).

## Findings, Corrections, and Reruns

T6-D017 corrected the ARM64 C89 cross runner after TC-0007 exposed a missing
`src/fenv.c` link dependency. T6-D022 corrected CRLF/LF comparison in the
generated FMA and complex-vector validators after exact CI failed before
product behavior. T6-D023 corrected the selected TinyCC ARM64 private complex
helper ABI after a native access violation; the retained automatic backtrace
identified `wcrt_check_exact` at `tests/c99/complex.c:101` and `main` at line
253. T6-D024--D026 added automatic built-in/GDB diagnostics, corrected their
exit-status enforcement, and made the DLL complex consumer prove exact dynamic
exports. T6-D027 retained dependency provenance and the ARM64 companion in the
named architecture artifact.

The final exact-revision rerun passed all three native C89 and 30-case
C99/compatibility aggregates, consumers, startup tests, source-quality gate,
and package assembly. ARM64 GDB remains an explicit `UnsupportedTarget`; the
TinyCC built-in trace is Pass and provides the required automatic failure
diagnostic on that runner.

The local Release rebuild initially used sibling repository compilers whose
x64/ARM64 executable hashes did not match CI. Those outputs were rejected as
provenance evidence. The retained candidate was rebuilt from the three exact
release archives and each extracted `tcc.exe` hash was verified against the
CI provenance before build and test.

WPM verification reached the trust check with repository-local `WPM_DATA_DIR`
and failed because the package is unsigned. Package structure Pass and WPM
cryptographic verification Fail remain separate.

## Commands

The controlled verification entry points were:

```powershell
./tools/test-tinycc-capabilities.ps1 -Architecture <arch> -TinyCc <tcc>
./tools/run-c89-tests.ps1 -Architecture <arch> -TinyCc <tcc>
./tools/run-extension-tests.ps1 -Architecture <arch> -TinyCc <tcc>
./tools/build-wcrt.ps1 -Architecture <arch> -Configuration <Debug-or-Release> -TinyCc <tcc> -Version 1.0.0-rc.1
./tools/test-built-libraries.ps1 -Architecture <arch> -Configuration <configuration> -TinyCc <tcc> [-CompileOnly]
./tools/test-startup-objects.ps1 -Architecture <arch> -Configuration <configuration> -TinyCc <tcc>
./tools/test-windows-2000-imports.ps1 -Binary <x86-wcrt.dll>
./tools/build-wpm-package.ps1 -Version 1.0.0-rc.1 -Configuration Release -Wpm 'C:\Program Files\WPM\wpm.exe'
./wsp/tools/New-ArtifactChecksum.ps1 -Path <package> -OutputPath <SHA256SUMS>
./wsp/tools/tests/run-tests.ps1
./tests/verify-traceability.ps1
./tests/c99/Verify-C99ConformanceProfile.ps1
./tests/c89/run-tc-0016.ps1
git diff --check
```
