# WSP Adoption Record

**Content type:** Project adoption record

**Project:** WCRT

**WSP baseline:** Immutable commit `2198ccab08f969a789448767fe7017b774369adc`

**Submodule path:** `wsp/`

**Pinned commit:** `2198ccab08f969a789448767fe7017b774369adc`

**Status:** Adopted

**Approval:** Adoption commit
`9be9216ee3f3c72dccad235a84083d4dfa8f9287`

## Common Baseline

| Requirement set or practice | Applicability | Project artifact or scope |
| --- | --- | --- |
| Common requirements management | Yes | `docs/REQUIREMENTS.md` and `docs/req-*.md` |
| WSP software lifecycle | Yes | Repository history, `ROADMAP.md`, CI, and release workflow |
| Project process | Yes | Repository review, issue, CI, and release practices |
| Documentation requirements | Yes | Project-controlled documentation and future release PDF |
| Documentation style and identifiers | Yes | Markdown, TeX, source documentation, and stable identifiers |
| Testing requirements | Yes | `docs/tc-*.tex`, `tests/`, CI, and generated reports |

## Selected Profiles

| Profile | Selected | Project scope or rationale |
| --- | --- | --- |
| Personal process | Yes | C99-to-1.0.0 and later requirement-sized maintainer work; records and data governance are defined in `docs/PROJECT-PROCESS.md`. |
| Security/DFS | Yes | Runtime behavior, build integrity, releases, and dependency trust |
| C source style | Yes | Project-owned headers and C sources |
| PowerShell style | Yes | Project-owned build, test, packaging, and reporting automation |
| CMake style | No | WCRT does not currently use CMake. |
| Windows version resources | Yes | Project-owned release DLLs |
| Windows code signing and Defender | Yes | Publicly distributed Windows DLLs and WPM packages |
| Common tools | Yes | Pinned traceability, source-quality, self-test, and checksum tools |

## Requirement Dispositions

### Common requirements management

| WSP requirement | Disposition | Project artifact | Rationale or notes |
| --- | --- | --- | --- |
| `WSP-REQM-0001` | Applicable | `docs/REQUIREMENTS.md`, `docs/req-*.md` | Stable project requirement identifiers are used. |
| `WSP-REQM-0002` | Tailored | `docs/req-*.md` | Each stable C89 conformance-unit record enumerates multiple related observable obligations. |
| `WSP-REQM-0003` | Applicable | `docs/req-*.md` | Requirement documents record scope, dependencies, and rationale. |
| `WSP-REQM-0004` | Applicable | `docs/tc-*.tex`, `tests/c89/manifest.md` | Verification is planned by requirement and test case. |
| `WSP-REQM-0005` | Applicable | `tests/verify-traceability.ps1` | Traceability is checked automatically. |
| `WSP-REQM-0006` | Applicable | `docs/WSP-ADOPTION.md` | This record controls the adopted baseline and dispositions. |
| `WSP-REQM-0007` | Applicable | `docs/WSP-ADOPTION.md` | Tailoring is explicit in this controlled record. |
| `WSP-REQM-0008` | Applicable | Baseline history in this record | WSP baseline upgrades include a controlled change-impact review. |
| `WSP-REQM-0009` | Applicable | Git history and requirement documents | Git preserves controlled requirement history. |
| `WSP-REQM-0010` | Applicable | Git tags and GitHub releases | Releases identify immutable source baselines. |

### Project process

| WSP requirement | Disposition | Project artifact | Rationale or notes |
| --- | --- | --- | --- |
| `WSP-PROC-0001` | Applicable | `docs/PROJECT-PROCESS.md` | The lifecycle, records, tailoring, and improvement path are defined. |
| `WSP-PROC-0002` | Applicable | `docs/PROJECT-PROCESS.md` | Maintainer, implementer, reviewer, verification, release, security, and improvement responsibilities are assigned. |
| `WSP-PROC-0003` | Applicable | `ROADMAP.md`, `docs/C99-1.0-WORK-PLAN.md`, GitHub issues and milestones | Work is planned proportionally to project risk and scope. |
| `WSP-PROC-0004` | Applicable | Git history and pull-request review | Project changes are version controlled and reviewable. |
| `WSP-PROC-0005` | Applicable | Pull-request review and required CI | Changes are reviewed before integration. |
| `WSP-PROC-0006` | Applicable | GitHub issues and test failures | Issues and defects are tracked with their corrective changes. |
| `WSP-PROC-0007` | Deferred | Future release-readiness record | Formal readiness evidence is not yet consolidated. |
| `WSP-PROC-0008` | Applicable | Git tags and GitHub releases | Approved releases establish immutable baselines. |
| `WSP-PROC-0009` | Applicable | GitHub issues and releases | Public support and corrective releases use repository records. |
| `WSP-PROC-0010` | Deferred | Future retrospective record | Periodic improvement review requires explicit evidence. |

### Personal process

| WSP requirement | Disposition | Project artifact | Rationale or notes |
| --- | --- | --- | --- |
| `WSP-PSP-0001` | Applicable | `docs/PROJECT-PROCESS.md` | The selected personal lifecycle covers planning through postmortem. |
| `WSP-PSP-0002` | Applicable | `docs/C99-1.0-WORK-PLAN.md`, `docs/work/` | Tranches require size, effort, forecast, assumptions, and uncertainty. |
| `WSP-PSP-0003` | Applicable | `docs/PROJECT-PROCESS.md`, `docs/work/` | Focused effort is recorded by phase and interruptions are excluded. |
| `WSP-PSP-0004` | Applicable | `docs/C99-1.0-WORK-PLAN.md`, `docs/work/` | The work plan defines defect fields and classification. |
| `WSP-PSP-0005` | Applicable | `docs/PROJECT-PROCESS.md`, `docs/work/` | Each tranche uses a personal review checklist before verification. |
| `WSP-PSP-0006` | Applicable | `docs/C99-1.0-WORK-PLAN.md` | Quality activities and early-removal checks are planned per tranche. |
| `WSP-PSP-0007` | Applicable | `docs/PROJECT-PROCESS.md`, `docs/work/` | Tranche and release postmortems compare estimates and actuals. |
| `WSP-PSP-0008` | Applicable | `docs/PROJECT-PROCESS.md`, postmortems | Evidence-based checklist and process improvements use normal review. |
| `WSP-PSP-0009` | Applicable | `docs/PROJECT-PROCESS.md` | Access, retention, aggregation, and prohibited comparative use are defined. |

### Documentation

| WSP requirement | Disposition | Project artifact | Rationale or notes |
| --- | --- | --- | --- |
| `WSP-DOC-0001` | Deferred | Future release-documentation pipeline | A consolidated release PDF is not yet published. |
| `WSP-DOC-0002` | Deferred | Future documentation manifest | Controlled PDF document ordering is not yet implemented. |
| `WSP-DOC-0003` | Deferred | Future release PDF | Reproducible PDF identity is not yet implemented. |
| `WSP-DOC-0004` | Deferred | Future release PDF | Required PDF navigation is not yet implemented. |
| `WSP-DOC-0005` | Deferred | Future documentation configuration | Shared release-document presentation is not yet implemented. |
| `WSP-DOC-0006` | Deferred | Future documentation pipeline | Markdown authority over generated release documents is not yet formalized. |
| `WSP-DOC-0007` | Deferred | Future documentation pipeline | Release-document build isolation is not yet implemented. |
| `WSP-DOC-0008` | Deferred | Future documentation tests | Required negative build behavior is not yet verified. |
| `WSP-DOC-0009` | Deferred | Future PDF verification | Automated and visual release-PDF checks are not yet implemented. |
| `WSP-DOC-0010` | Deferred | Future release PDF | Required embedded and displayed metadata is not yet implemented. |
| `WSP-DOC-0011` | Deferred | Future release workflow | A PDF digest is not yet published. |
| `WSP-DOC-0012` | Deferred | Future release workflow | PDF provenance attestation is not yet generated. |
| `WSP-DOC-0013` | Not applicable | This adoption record | WCRT has no present requirement for a PAdES-signed documentation PDF. |

### Testing

| WSP requirement | Disposition | Project artifact | Rationale or notes |
| --- | --- | --- | --- |
| `WSP-TEST-0001` | Applicable | `docs/REQUIREMENTS.md`, `tests/c89/manifest.md` | Implemented requirements map to verification. |
| `WSP-TEST-0002` | Applicable | `docs/tc-*.tex` | Test specifications are controlled with the source. |
| `WSP-TEST-0003` | Applicable | `docs/tc-*.tex` | Test cases record inputs, procedure, and expected results. |
| `WSP-TEST-0004` | Applicable | `docs/tc-*.tex`, `tests/c89/run-tc-*.ps1` | Specifications define behavior implemented by runners. |
| `WSP-TEST-0005` | Applicable | `tests/c89/run-tc-*.ps1` | Tests use repeatable scripted execution. |
| `WSP-TEST-0006` | Applicable | `.github/workflows/build.yml` | CI executes project verification automatically. |
| `WSP-TEST-0007` | Applicable | Test execution reports | Report generation records execution context. |
| `WSP-TEST-0008` | Applicable | Test runners and reports | Automated outcomes distinguish pass, fail, and execution errors. |
| `WSP-TEST-0009` | Applicable | CI logs and generated reports | Failures preserve actionable output. |
| `WSP-TEST-0010` | Applicable | `tools/generate-test-report.ps1`, `tools/generate-test-execution-report.ps1` | Reports are generated from controlled inputs. |
| `WSP-TEST-0011` | Applicable | `tests/verify-traceability.ps1` | CI validates requirement-to-test links. |
| `WSP-TEST-0012` | Applicable | `.github/workflows/build.yml` | Supported configurations are continuously tested. |
| `WSP-TEST-0013` | Applicable | `.github/workflows/build.yml` | Release jobs cover x86, x64, and ARM64 artifacts. |
| `WSP-TEST-0014` | Deferred | Future evidence-retention policy | CI and release artifacts exist, but retention is not formally defined. |
| `WSP-TEST-0015` | Applicable | `docs/tc-*.tex`, `docs/TEST-STRATEGY.md` | Every controlled test identifies its selected technique or explains non-applicability. |

### Security and DFS

| WSP requirement | Disposition | Project artifact | Rationale or notes |
| --- | --- | --- | --- |
| `WSP-SEC-0001` | Deferred | Future WCRT Design for Security | Security scope requires a controlled record. |
| `WSP-SEC-0002` | Deferred | Future WCRT Design for Security | A controlled DFS is not yet maintained. |
| `WSP-SEC-0003` | Deferred | Future WCRT Design for Security | Trust boundaries and assumptions require documentation. |
| `WSP-SEC-0004` | Deferred | Future WCRT Design for Security | Threat analysis requires documentation. |
| `WSP-SEC-0005` | Deferred | Future WCRT Design for Security | Derived security requirements require documentation and traceability. |
| `WSP-SEC-0006` | Applicable | Runtime requirements and tests | Input bounds, resource behavior, and failure cases are verified per implemented API. |
| `WSP-SEC-0007` | Not applicable | Runtime library scope | WCRT has no authorization boundary or privileged service role. |
| `WSP-SEC-0008` | Applicable | Release-key handling and WPM verification workflow | Package trust uses controlled keys; secrets are not committed. |
| `WSP-SEC-0009` | Applicable | Submodule gitlink, CI, and dependency checks | Dependencies and build inputs are pinned or verified. |
| `WSP-SEC-0010` | Not applicable | Runtime library scope | WCRT does not retain user data or implement application security logging. |
| `WSP-SEC-0011` | Applicable | Runtime requirements and tests | Error and recovery behavior is verified for implemented interfaces. |
| `WSP-SEC-0012` | Applicable | `tests/` and `.github/workflows/build.yml` | Security-relevant behavior is exercised by automated verification. |
| `WSP-SEC-0013` | Applicable | Pull requests, CI, and GitHub issues | Review and defect handling use controlled repository workflows. |
| `WSP-SEC-0014` | Deferred | Future vulnerability-response policy | Reporting, triage, disclosure, and response timelines require documentation. |

### C source style

| WSP requirement | Disposition | Project artifact | Rationale or notes |
| --- | --- | --- | --- |
| `WSP-CSTYLE-0001` | Applicable | `include/`, `src/`, `docs/req-0016-source-documentation.md` | Project-owned C files require file documentation. |
| `WSP-CSTYLE-0002` | Applicable | `include/`, `src/`, `docs/req-0016-source-documentation.md` | Project-owned functions require Doxygen documentation. |
| `WSP-CSTYLE-0003` | Applicable | `include/`, `src/`, `docs/req-0016-source-documentation.md` | Public entities require Doxygen documentation. |
| `WSP-CSTYLE-0004` | Applicable | `include/`, `src/`, `docs/req-0016-source-documentation.md` | WCRT enforces an 80-column physical-line limit. |
| `WSP-CSTYLE-0005` | Applicable | `tests/c89/run-tc-0016.ps1` | Source documentation and line length are enforced automatically. |

### Windows version resources

| WSP requirement | Disposition | Project artifact | Rationale or notes |
| --- | --- | --- | --- |
| `WSP-WINRES-0001` | Applicable | `tools/add-win32-resources.ps1` | Release DLLs receive VERSIONINFO resources. |
| `WSP-WINRES-0002` | Applicable | `tools/add-win32-resources.ps1` | Version-resource source is generated during the build. |
| `WSP-WINRES-0003` | Applicable | `tools/wcrt-version.ps1` | Versions are converted to Windows numeric form. |
| `WSP-WINRES-0004` | Applicable | `tools/wcrt-version.ps1` | Product and file versions derive from the release identity. |
| `WSP-WINRES-0005` | Applicable | `tools/add-win32-resources.ps1` | Required string information is generated. |
| `WSP-WINRES-0006` | Applicable | `tools/add-win32-resources.ps1` | Resource identity describes WCRT binaries. |
| `WSP-WINRES-0007` | Applicable | `tools/add-win32-resources.ps1` | File type and operating system are generated explicitly. |
| `WSP-WINRES-0008` | Applicable | `tools/add-win32-resources.ps1` | File flags derive from the build identity. |
| `WSP-WINRES-0009` | Applicable | `tools/add-win32-resources.ps1` | Language and code page are explicit. |
| `WSP-WINRES-0010` | Applicable | Build matrix and resource generator | Resource identity follows the target architecture. |
| `WSP-WINRES-0011` | Applicable | `tools/add-win32-resources.ps1` | Legal and public project information is generated. |
| `WSP-WINRES-0012` | Deferred | Future binary-resource verification | Automated checks do not yet verify every required field in the built DLL. |

### Windows code signing and Defender

| WSP requirement | Disposition | Project artifact | Rationale or notes |
| --- | --- | --- | --- |
| `WSP-SIGN-0001` | Deferred | Future release signing plan | The complete signing plan is not yet documented. |
| `WSP-SIGN-0002` | Deferred | Future release documentation | Trust layers require explicit documentation. |
| `WSP-SIGN-0003` | Deferred | Future release signing plan | Authenticode identity controls require documentation. |
| `WSP-SIGN-0004` | Deferred | Future release workflow | Release PE files are not yet Authenticode signed. |
| `WSP-SIGN-0005` | Deferred | Future release workflow | RFC 3161 timestamping is not yet implemented. |
| `WSP-SIGN-0006` | Not applicable | Current platform baseline | No legacy SHA-1 signing exception is selected. |
| `WSP-SIGN-0007` | Deferred | Future release workflow | The complete signing and packaging order is not yet implemented. |
| `WSP-SIGN-0008` | Deferred | Future release workflow | Authenticode verification is not yet a release gate. |
| `WSP-SIGN-0009` | Deferred | Future release-trust record | Exact signed-artifact identity is not yet retained. |
| `WSP-SIGN-0010` | Deferred | Future release workflow | Final artifacts are not yet scanned with retained Defender evidence. |
| `WSP-SIGN-0011` | Deferred | Future release workflow | Defender detection handling is not yet a formal gate. |
| `WSP-SIGN-0012` | Deferred | Future vulnerability-response policy | Windows trust reports are not yet formally classified. |
| `WSP-SIGN-0013` | Deferred | Future vulnerability-response policy | False-positive investigation is not yet formalized. |
| `WSP-SIGN-0014` | Deferred | Future vulnerability-response policy | Microsoft submissions are not yet formally tracked. |
| `WSP-SIGN-0015` | Applicable | Project release and support practice | Protection evasion and broad exclusions are not accepted release responses. |
| `WSP-SIGN-0016` | Deferred | Future release documentation | Publisher identity and verification guidance require completion. |
| `WSP-SIGN-0017` | Deferred | Future release signing plan | Certificate lifecycle and compromise response require documentation. |
| `WSP-SIGN-0018` | Deferred | Future release-trust record | Trust-evidence retention is not yet defined. |

### Common tools

| WSP requirement | Disposition | Project artifact | Rationale or notes |
| --- | --- | --- | --- |
| `WSP-TOOL-0001` | Applicable | `.github/workflows/build.yml`, `tests/test-logging.ps1` | CI invokes tools and the logging adapter from the pinned WSP submodule. |
| `WSP-TOOL-0002` | Applicable | WSP tool invocations | WCRT passes explicit project-owned paths. |
| `WSP-TOOL-0003` | Applicable | WSP common-tool self-tests | Selected tools have deterministic file-based inputs. |
| `WSP-TOOL-0004` | Applicable | WSP common-tool self-tests | Positive and negative exit behavior is tested. |
| `WSP-TOOL-0005` | Applicable | WSP validation output | Diagnostics identify affected artifacts and rules. |
| `WSP-TOOL-0006` | Applicable | `output/` and `release-assets/` | Generated files remain outside the submodule. |
| `WSP-TOOL-0007` | Applicable | Selected tool interfaces | No selected invocation receives or emits secrets. |
| `WSP-TOOL-0008` | Applicable | `wsp/tools/tests/run-tests.ps1` | Pinned common-tool self-tests run in CI. |
| `WSP-TOOL-0009` | Applicable | `.github/workflows/build.yml` | Workflows use maintained v6 GitHub actions on GitHub-hosted Windows runners; self-hosted runner exceptions are not currently required. |

## Tailoring Decisions

### WSP-REQM-0002 — Requirement atomicity

- **Disposition:** Tailored
- **Rationale:** WCRT's established identifiers represent complete public C89
  conformance units, normally one standard header, with their observable
  obligations enumerated in a single controlled record.
- **Impact:** A requirement may map to multiple behaviors and checks rather
  than one normative sentence.
- **Compensating control:** Every obligation is explicit in the singular
  Requirement section, each record traces to a controlled test case, and the
  runners exercise the complete conformance unit.
- **Owner:** WCRT maintainer
- **Completion condition:** N/A
- **Approval:** Review and merge of the `wsp_inclusion` branch

### Deferred requirements

- **Disposition:** Deferred
- **Rationale:** Initial adoption records the current project state without claiming controls that have not yet been implemented or evidenced.
- **Impact:** The deferred controls cannot be treated as release evidence until completed.
- **Compensating control:** Existing source control, CI, automated testing, GitHub release records, and WPM verification remain in force.
- **Owner:** WCRT maintainer
- **Completion condition:** Each deferred row must be implemented, evidenced, and changed to Applicable, or separately tailored with approval.
- **Approval:** Review and merge of the `wsp_inclusion` branch

### Non-applicable requirements

- **Disposition:** Not applicable
- **Rationale:** CMake, PAdES, authorization, and application data logging are outside WCRT's current project or product scope.
- **Impact:** WCRT makes no compliance claim for these profiles or controls.
- **Compensating control:** Reassess applicability when the build system, release format, product boundary, or project governance changes.
- **Owner:** WCRT maintainer
- **Completion condition:** N/A
- **Approval:** Review and merge of the `wsp_inclusion` branch

## Baseline History

| Date | WSP baseline | Project change | Summary |
| --- | --- | --- | --- |
| 2026-07-26 | `3ac1b461c06071e15da6c6f2c44c253a4d9220ef` | `wsp_inclusion` | Initial WSP adoption at `tools/wsp/` |
| 2026-07-26 | `44f591d6416d18252596d8c9fb45f8fbaa65d08a` | WSP baseline update | Adopt latest upstream WSP, including the 1.0.0 release, action-runtime currency requirements, and shared logging tools. |
| 2026-07-29 | `0f66aa65fd820799468818ae57897f2940fb6037` | WSP baseline update | Add native standard-input testing guidance. Impact review found no requirement-disposition or logging-interface changes; existing WCRT logging integration remains compatible. |
| 2026-08-03 | `2198ccab08f969a789448767fe7017b774369adc` | WSP baseline update and relocation | Add portable C logger fallbacks and tests, adopt intervening upstream guidance, and move the checkout from `tools/wsp/` to the project-root `wsp/` path. |
| 2026-08-04 | `2198ccab08f969a789448767fe7017b774369adc` | C99-to-1.0.0 planning change | Select the personal-process profile and define the project process, measured C99 work plan, and 1.0.0 release boundary. |
