# WCRT documentation guide

This directory contains current contracts, verification records, architecture
decisions, release controls, planning documents, and retained historical
evidence. Start with the current-contract documents below; plans and old
readiness records are useful context but are not the implemented API inventory.

## Current contract

- [Requirements index](REQUIREMENTS.md) — authoritative map from each supported
  facility to its requirement and test case.
- [C99 conformance profile](c99-conformance-profile.md) — implemented C99
  classifications and target status.
- [Implementation-defined behavior](implementation-defined.md) — choices the
  C standard leaves to WCRT.
- [C89 deviations](c89-deviations.md) — accepted and resolved deviations from
  the C89/C90 baseline.
- [C99 deviations](c99-deviations.md) — known differences from the selected
  C99 contract.
- [ARM64 support](arm64-support.md) — architecture boundary and build model.
- [Test strategy](TEST-STRATEGY.md) — verification levels, manifests, and
  evidence rules.

Compatibility inventories are maintained beside their executable tests:

- [Microsoft compatibility manifest](../tests/mscompat/manifest.md)
- [POSIX compatibility manifest](../tests/posix/manifest.md)
- [WCRT extension manifest](../tests/wcrt/manifest.md)

The POSIX manifest describes a deliberately bounded Windows portability layer.
It is not a POSIX conformance claim and does not define `_POSIX_VERSION`.

## Design and release policy

- [Architecture decision records](adr-0001-separate-optional-pe-startup-objects.md)
  begin with ADR-0001 and record durable design decisions through ADR-0007.
- [Project process](PROJECT-PROCESS.md) and the
  [WSP adoption record](WSP-ADOPTION.md) define the tailored engineering
  process.
- [Release process](release-process.md) defines build, signing, publication,
  support, and rollback controls.
- Tagged builds use `tools/Build-ReleaseDocumentation.ps1` to generate three
  architecture-specific test-execution PDFs, a Doxygen API-reference PDF, and
  an offline HTML/LaTeX/PDF documentation bundle. The HTML reference is also
  published to GitHub Pages.
- [Security design](security/design-for-security.md) records the threat and
  dependency model.
- [Windows signing plan](windows-signing-plan.md) retains the deferred
  Authenticode design; it is not evidence that DLLs are Authenticode-signed.

## Requirements and tests

Each `req-NNNN-*.md` file specifies one controlled requirement. The matching
`tc-NNNN-*.tex` file specifies its verification. Do not infer support merely
from a header or roadmap entry; the requirement, implementation, and test must
all exist and pass.

The repository verifies that these sets remain aligned:

```powershell
./tests/verify-traceability.ps1
./tests/c89/run-tc-0016.ps1
```

Behavioral aggregates are run with an architecture-matched TinyCC:

```powershell
./tools/run-c89-tests.ps1 `
  -Architecture x64 `
  -TinyCc $env:TCC_HOME/tcc.exe
./tools/run-extension-tests.ps1 `
  -Architecture x64 `
  -TinyCc $env:TCC_HOME/tcc.exe
```

## Planning and retained evidence

The following are planning or historical records. Their unchecked items and
status lines describe the baseline at which they were written, not necessarily
the current repository:

- [C99 1.0 work plan](C99-1.0-WORK-PLAN.md) and its retained prompts
- [WCRT 1.1.0 compatibility roadmap](WCRT-1.1.0-ROADMAP.md)
- `work/` tranche records
- `evidence/` retained build and dependency evidence
- `release/` release-candidate readiness records

Use the root [roadmap](../ROADMAP.md) for long-range direction and the
[requirements index](REQUIREMENTS.md) for current implementation status.
