# C99 T6 Local Verification Record

**Content type:** Retained local verification evidence

**Candidate source:** `3fa0b1a3a22bbd93ff9185dbb24689d83e057e56`

**Execution date:** 2026-08-17 (America/Chicago)

**Host:** Microsoft Windows 10.0.26200, x64 OS, x64 process, PowerShell 7.6.3

**Decision:** Local x86/x64 and cross-build scope Pass; native ARM64 and
release-trust scope are not complete

## Dependency Baseline

The machine-readable dependency record is
`docs/evidence/c99-t6/dependency-provenance.json`. The selected WSP gitlink is
`2198ccab08f969a789448767fe7017b774369adc`; TinyCC is
`0.9.28-rc.1442+2474e1c2` at source revision
`d5c02f0fcdfdf75265d38df6ff9db2f8067367ac`; WPM is 1.0.16; and cv2pdb is
0.54.0 at source revision `25cfecaad700550a49cd7ddbf7773254ea438d07`.
Executable and release-key hashes are in the machine-readable record.

The exact GitHub Actions workflow was not run because no push or other
external write was authorized. Native runner image resolution and CI-produced
dependency JSON are therefore Unknown.

## Gate Results

| Gate | x86 | x64 | ARM64 | Evidence and boundary |
| --- | --- | --- | --- | --- |
| TinyCC language/data model | Pass | Pass | Pass (compile) | Ten probes per target: seven Supported and three controlled compiler-blocked ExpectedFail pragmas; no unexpected probe failure |
| Header self-containment/repetition/mixed order | Pass | Pass | Pass (compile) | TC-0042; all 24 C99 headers, all 15 C89 headers, two C99 mixed orders, one C89 mixed order |
| C89 TC-0001--TC-0015 | 15/15 Pass native | 15/15 Pass native | 15/15 Pass compile/link; native Unknown | ARM64 outputs are PE `0xAA64` and explicitly say `DeferredToArm64CI` |
| C99 TC-0019, 0021--0033, 0035--0042 | 22/22 Pass native | 22/22 Pass native | Native Unknown | ARM64 TC-0042 compile matrix passes; behavior is not inferred |
| Microsoft compatibility TC-0020, 0043--0049 | 8/8 Pass native | 8/8 Pass native | Native Unknown | No ARM64 behavior Pass is claimed |
| C89 isolation and constant-expression/type checks | Pass | Pass | Header/ABI compile Pass; behavior Unknown | Controlled header/presence tests and TC-0042 |
| Clause/facility inventory | Pass | Pass | Profile complete; required behavior Unknown where shown | 25 clauses, 24 headers, 75 facility rows; profile validator Pass |
| WSP common tools | Pass | Pass | N/A host gate | PowerShell syntax, logging, traceability/evidence negative tests, report, source-quality, warning, checksum, and documentation negative test Pass |
| Traceability | Pass | Pass | Pass inventory | 48 requirements, 48 test specifications, 48 implementations |
| Source quality | Pass | Pass | Pass source | TC-0016: 181 C/header files, zero violations, Doxygen 1.17.0 |
| Debug and Release builds | Pass | Pass | Pass compile/link | Six DLL/static/startup build sets produced |
| Static and DLL consumers | Pass native | Pass native | Pass compile/link; native Unknown | Debug and Release; C89 and complex consumers |
| Console and GUI startup | Pass native | Pass native | Compile/link Pass; native Blocked/Unknown | TC-0017/TC-0018 on Debug and Release |
| Windows 2000 imports | Pass | N/A | N/A | x86 Debug and Release machine `0x014C`; Release DLL hash `f5a6dee9...` has 27 allowlisted `kernel32.dll` imports and no unexpected import |
| Package structure | Pass | Pass | Pass | One multi-architecture unsigned archive; required entries and complex profiles checked by `build-wpm-package.ps1` |
| WPM signature verification | Fail | Fail | Fail | WPM 1.0.16 exits 1: `unsigned packages are not allowed` |
| Authenticode/timestamp | Fail | Fail | Fail | Each Release DLL is `NotSigned`; no signer or timestamp exists |
| Defender | Informational clean | Informational clean | Informational clean | Engine 1.1.26070.7 / intelligence 1.457.196.0 found no threats in the unsigned DLLs and package; final signed bytes do not exist, so the release gate is Unknown |
| Installation/removal/rollback | Unknown | Unknown | Unknown | Not executed because it changes Program Files/registry and no separate authority was given |
| Exact-revision native CI | Unknown | Unknown | Unknown | No push/workflow dispatch was authorized; local x86/x64 evidence is separate |

## Build Identities

| Architecture | Configuration | DLL SHA-256 |
| --- | --- | --- |
| x86 | Debug | `6810c7452fa5bd24324f73017886c113a91d979aee78a2e666653392b0ffcbcb` |
| x86 | Release | `f5a6dee96bb2c12342bcfe7ed9cd77aca86d88fe0bf328006a470e4c4d365ace` |
| x64 | Debug | `4dc021fdffde9055be94e4a4bc715d99706358b9be0f070527b58e9d43b0acd9` |
| x64 | Release | `f9160b9f42531af614867c059e707e713e9d4cdced1f06e0a9f8c94e104fa78d` |
| ARM64 | Debug | `1df49f6287e1f208ce7635a6cfb6f942eb60f04e82d2d37c27567fbf63657685` |
| ARM64 | Release | `f70c3a65668f5dcb457b56204d8583f158df585844e6c989117582fb16a81dd5` |

All three Release DLLs report file/product version
`1.0.0-rc.1.17+3fa0b1a3`. The exact package and internal artifact hashes are in
`docs/evidence/c99-t6/release-candidate-manifest.json`.

## Findings and Reruns

The first frozen candidate, source
`23497525bd0be03b0d53d79686376e93ade94046`, failed ARM64 C89 TC-0007 during
link with these exact diagnostics:

```text
tcc: error: unresolved reference to 'fegetround'
tcc: error: unresolved reference to 'feraiseexcept'
tcc: error: unresolved reference to 'fegetexceptflag'
tcc: error: unresolved reference to 'fesetexceptflag'
```

T6-D017 records that the ARM64 cross runner omitted `src/fenv.c` after the T6
`fma` correction made that dependency necessary. The runner was corrected in
`3fa0b1a3a22bbd93ff9185dbb24689d83e057e56`. All 15 ARM64 C89 cross units then
passed, and x86/x64 TC-0007, TC-0035, and TC-0036 passed focused reruns. The
complete exact-candidate x86/x64 aggregates and ARM64 cross aggregate were then
rerun and passed within their stated scopes.

An attempted ARM64 native extension aggregate on the x64 host produced 27
process-launch errors and three compile/document Pass results. T6-D018 records
this as an invalid verification procedure, not a product Pass or Fail. The
failed aggregate is not used to infer ARM64 behavior; native ARM64 remains
Unknown.

The first WPM verification attempt could not use the sandbox-disallowed
`C:\ProgramData\WPM` temporary directory. Repeating with repository-local
`WPM_DATA_DIR` reached the package trust check and failed for the substantive
reason that the candidate is unsigned. Package structure Pass and WPM
cryptographic verification Fail are retained separately.

Two unused bootstrap dependency scripts still inventory absent miniz and
libsodium directories. They are not invoked by the workflow and do not affect
the pinned build, but their freshness output is not provenance and is excluded
from Pass evidence.

## Commands

The verification used the repository entry points below with each exact
architecture compiler path:

```powershell
./tools/test-tinycc-capabilities.ps1 -Architecture <arch> -TinyCc <tcc>
./tools/run-c89-tests.ps1 -Architecture <x86-or-x64> -TinyCc <tcc>
./tools/run-c89-arm64-cross.ps1 -TinyCc <arm64-tcc>
./tools/run-extension-tests.ps1 -Architecture <x86-or-x64> -TinyCc <tcc>
./tests/c99/run-tc-0042.ps1 -TinyCc <arm64-tcc>
./tools/build-wcrt.ps1 -Architecture <arch> -Configuration <Debug-or-Release> -TinyCc <tcc> -Version 1.0.0-rc.1
./tools/test-built-libraries.ps1 -Architecture <arch> -Configuration <configuration> -TinyCc <tcc> [-CompileOnly]
./tools/test-startup-objects.ps1 -Architecture <arch> -Configuration <configuration> -TinyCc <tcc>
./tools/test-windows-2000-imports.ps1 -Binary <x86-wcrt.dll>
./tools/build-wpm-package.ps1 -Version 1.0.0-rc.1 -Configuration Release -Wpm 'C:\Program Files\WPM\wpm.exe'
./wsp/tools/tests/run-tests.ps1
./tests/verify-traceability.ps1
./tests/c99/Verify-C99ConformanceProfile.ps1
./tests/c89/run-tc-0016.ps1
git diff --check
```
