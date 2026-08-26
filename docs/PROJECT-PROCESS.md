# WCRT Project Process

**Content type:** Project process

**Status:** Adopted

**Approval:** Adopted by commit
`81081dd92f6e6f797208c412cab09b079160fd7a`

## Scope and Lifecycle

WCRT uses the WSP lifecycle iteratively. `ROADMAP.md` defines release scope and
ordering; controlled `docs/req-NNNN-*.md` records specify behavior; ADRs record
durable design decisions; project source implements the requirements; pull
request review and CI provide review and verification; a release-readiness
record approves a release; and a postmortem updates estimates and process
checklists.

Work is divided into requirement-sized tranches. A tranche is complete only
when its requirements, test specifications, implementation, review findings,
traceability, and supported-target evidence are complete. A passing
implementation without those records is not complete. The completed
C99-to-1.0.0 program is retained in `docs/C99-1.0-WORK-PLAN.md`; selected
scope and ordering for the active compatibility program are in
`docs/WCRT-1.1.0-ROADMAP.md`.

## Responsibilities

One maintainer may hold multiple roles. Pull-request review provides the
additional review available to this personal project.

| Responsibility | Accountable role | Required record or evidence |
| --- | --- | --- |
| Product scope and priorities | Maintainer | Roadmap and work plan |
| Requirements and architecture | Maintainer | Requirement records and ADRs |
| Implementation and personal review | Implementer | Change, checklist, and work log |
| Independent review | Pull-request reviewer | Review disposition |
| Verification | Maintainer and CI | Controlled tests and retained results |
| Configuration and dependencies | Maintainer | Git baseline, pinned inputs, provenance |
| Security response | Maintainer | Issue, DFS update, and release disposition |
| Release approval | Maintainer | Release-readiness record |
| Process improvement | Maintainer | Postmortem and approved process change |

The author shall not close a material review finding without a correction,
approved deferral, or accepted-risk record.

## Work Control

Before implementation, each tranche shall identify scope, dependencies,
completion criteria, risks, estimates, planned reviews, verification, and
release effect. The active release roadmap is
`docs/WCRT-1.1.0-ROADMAP.md`.

For 1.1.0 and later work, tranche records shall cover the WSP milestone-plan,
design, review, and closeout templates. These may be separate documents or
clearly identified sections of one project work record when consolidation does
not lose required information. The selected personal-process work log remains
the chronological source for estimates, effort, defects, review results, and
postmortem measures.

Changes to a baselined requirement, public interface, supported target,
security control, test obligation, or release artifact require an impact note
in the requirement, issue, pull request, or ADR. The note shall cover affected
implementation, tests, documentation, compatibility, security, schedule, and
evidence as applicable.

Issues and defects shall record the affected baseline, observed behavior,
priority, owner, disposition, and verification. For personal-process work, the
work log also records defect type, injection and removal phase when known, and
fix effort.

## Personal Process

The WSP personal-process profile applies to work performed by the maintainer on
the C99-to-1.0.0 program and later requirement-sized feature or corrective
tranches. Each tranche follows these phases:

1. Plan and estimate.
2. Specify or review requirements.
3. Design and record durable decisions.
4. Implement.
5. Perform a personal design, code, and test review.
6. Verify locally and in CI.
7. Record a postmortem and process improvements.

The approved record is a Markdown work log in `docs/work/`, named for the
tranche. It records planned and actual size, effort by phase, completion
forecast, interruptions excluded from effort, defects, checklist results, and
postmortem. Estimates are ranges until enough comparable WCRT history exists.

Personal measures are accessible to the maintainer and reviewers of the
project repository. They are retained with project history, used only for
planning, quality improvement, and release decisions, and shall not be used as
standalone performance rankings or cross-person comparisons. Public summaries
may aggregate tranche data but shall not attribute comparative measures.

## Review and Quality Checklist

Every tranche shall review, as applicable:

- requirement completeness and C99 clause traceability;
- C89-mode isolation and public-header self-containment;
- x86, x64, and ARM64 ABI differences;
- overflow, range, rounding, locale, stream-state, and error behavior;
- Windows 2000 x86 import compatibility;
- negative, boundary, regression, and cross-architecture tests;
- documentation of implementation-defined behavior and deviations; and
- source quality, warnings, dependency trust, and generated evidence.

## Release, Support, and Improvement

Before a release, the maintainer completes a project-owned copy of the WSP
release-readiness template. Version, source revision, artifacts, requirements,
tests, open risks, dependency baseline, signing, malware scan, checksums,
documentation, rollback, and support actions must be known. Failed or unknown
required gates cannot be represented as verified.

An approved deferred objective is outside the current release's completion and
verification claims and shall not also be listed as a required gate. The
release-readiness record shall give each deferral its impact, owner, target
release or completion condition, compensating control, and approval. Each
release decision shall close, revise, or explicitly carry forward every open
deferral. A failed or unknown required gate still prevents release approval.

GitHub issues are the public defect and vulnerability intake. The current
major release and its documented compatibility commitments are supported until
an end-of-support notice is published.

Each tranche postmortem compares estimate and actual results. The 1.0.0
postmortem additionally reviews escaped defects, CI failures, release evidence,
and candidate changes to this process, the personal checklist, and WSP.
