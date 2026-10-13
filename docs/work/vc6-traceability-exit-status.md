# Traceability success exit-status correction

## Plan, finding and review

VC6 source-route runners deliberately invoke failed compiler commands to prove
wrong-target/profile rejection. A subsequent verify-traceability invocation
prints a passing mapping count but exits with the earlier compiler's status 1.
Repeated three times during REQ-0091/0095 checks. The shared PowerShell validator
has explicit failure exit 1 but no explicit success exit; the wrapper reads
LASTEXITCODE, which the successful pure PowerShell validation does not update.

Estimate: one tool line, 10-20 minutes. Add explicit success exit 0 after the
retained passing result. Preserve all validation and failure branches. Check
success after a failed Git command in a fresh child PowerShell and failure on
an owned empty fixture repository. No runtime/API/package/import change or
dependency is planned; this corrects gate evidence rather than relaxing it.
Actual effort is not reliably timed.

## Verification and closeout

On 2026-10-01 a fresh child PowerShell returned zero after a failed Git command
followed by passing validation of all 94 mappings. A separate child validating
an owned empty fixture repository retained nonzero failure. Existing validation
branches/messages were unchanged. One source line was added, within estimate;
actual effort was not reliably timed. No gate was disabled or reset blindly.

On 2026-10-13 the WCRT wrapper was changed to invoke the validator in a fresh
PowerShell process. This isolates its native exit status without requiring a
new WSP gitlink or publishing the local WSP edit. The command preserves all
five manifest arguments as a PowerShell array and quotes repository/tool
paths. All 102 mappings pass; validator failures retain nonzero exit status.
