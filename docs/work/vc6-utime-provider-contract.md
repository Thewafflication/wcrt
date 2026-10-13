# File-time provider contract review

## Plan, design and estimate

Extend REQ-0048 verification during VC6 coverage review. Existing native tests
use distinct timestamps, but the isolated Windows provider accepts only equal
access/modification timestamps and does not inspect open arguments. Estimate:
60-100 fixture lines, 20-45 minutes. Capture each timestamp independently,
validate the baseline Windows open/update contract, and exercise path/open
failure categories, both timestamp fields and side-effect counts.

No runtime, ABI, import or public declaration change is planned. Preserve
native filesystem round trips and ARM64 compile/link-only qualification.
Review provider failure precedence and error capture before close; require
targeted TC-0048, quality, traceability and diff checks. Genuine VC6/reference
qualification remains Unknown. Actual effort is not reliably timed.

## Review and closeout

On 2026-10-01 direct native/provider and static/DLL TC-0048 checks passed on
x86/x64. ARM64 compiled/linked both fixtures and package consumers only.
Distinct epoch/maximum timestamps, independent invalid fields, missing-path
precedence, failed-open ownership and overwritten native error after close
passed. Provider arguments were inspected for every successful open/update.
TC-0016 passed all 339 files with zero violations; traceability retained 91
mappings and diff whitespace checks passed. The first quality invocation used
an unsupported TinyCc parameter; rerunning the actual parameterless runner
completed successfully. No runtime defect or behavior change resulted.
About 65 fixture lines were added, within estimate; effort was not timed.
