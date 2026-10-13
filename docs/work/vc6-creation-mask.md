# Process creation-mask tranche

## Plan and design

Implement REQ-0101 _umask with an initially zero process mask containing only
_S_IREAD/_S_IWRITE. Return the previous mask; ignore unsupported bits and retain
errno. Apply one mask snapshot to new low-level files through shared-open
permissions, covering _sopen/_creat and _open's existing default permissions.
Keep existing attributes, descriptor access flags and stream APIs unchanged.
Read suppression is ineffective on Windows; omission of effective write
permission creates readonly attributes as in REQ-0083. Concurrent mask changes
and mask/creation transaction ordering are outside this selected contract.

Estimate 20-35 runtime/header lines, 100-170 fixture lines and 45-70 minutes.
Review unsupported signed bits, previous-mask returns, validation before mask
application, new/existing file partitions and backend snapshots. Require native
all mode combinations, initial descriptor access, existing attribute retention,
provider snapshot/creation failure controls, x86/x64 packages, ARM64 cross-links,
supplementary Microsoft calls and existing _sopen/_creat regressions. Retain
quality/traceability/import gates. Additive export/global behavior requires
rebuilt packages; no host CRT, allocation or Windows import is added.
Genuine VC6/reference remains Unknown and actual effort is not yet measured.

Current [Microsoft _umask documentation](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/umask)
specifies previous-mask returns, ignored unsupported bits, Windows read-bit
limitations and low-level creation effects. Local MSVC 16 source inspection
corroborates masking unsupported bits, without copying vendor source or
qualifying version 12/reference behavior. The existing controlled _open API's
default creation permission convention is retained; full optional variadic
permission parity and fopen/fopen_s creation-mask effects remain open.

## Implementation and review

The presence fixture reproduced the missing _umask declaration. A volatile
process mask starts at zero; the setter filters read/write bits and returns
the prior value without touching errno. The shared-open backend snapshots only
for create flags, after requested-mode/sharing validation and before opening.
Its existing new-versus-existing classification and attribute failure cleanup
are retained. Descriptor access flags stay independent of effective permission.

Native x86/x64 checks passed all sixteen requested-mode/mask combinations,
read-bit ineffectiveness, unsupported/high/negative bits, initial read/write
descriptor access, existing writable truncation under a write mask, existing
readonly rejection and _open default/_creat integration. Provider controls
change subsequent snapshots to prove one read per creation, check invalid
inputs and non-creation paths never snapshot, and retain allocation/open/attribute
errors. The previous shared-creation baseline runs inside the provider test.
Static/DLL packages passed x86/x64; ARM64 compiled/linked only. Supplementary
MSVC 16 /Od and /O2 consumers called the mask setter through a pointer, created
a readonly file, restored the previous mask and cleaned the owned fixture.
All 87 input hashes matched the rebuilt runtime and current consumer inputs.

Personal review caught two new fixture locals placed in the preceding function
by an overly broad patch context; moved them into check_interfaces before
Microsoft compilation. Source quality passed 375 files, traceability passed
100 mappings, and unchanged Windows 2000 imports/diff checks passed. The private
mask query follows the existing export-all helper policy without joining the
supported public API inventory. Full native regressions are running before
closeout; concurrent mask updates and creation transaction ordering remain
unqualified and actual effort is untimed.
The full C89 run reproduced a missing new-module dependency in its explicit
minimal stdio source list. Updated every manually listed file backend consumer
(C89 stdio/stdin, C99 stdio/wide helpers, compatibility formatting/fileno and
ARM64 C89 cross runner) to include ms_umask.c. Whole-runtime builds had already
included it. Rerun affected aggregates and ARM64 cross checks before closeout.

## Closeout

Both native targets passed all 15 C89 and 82 extension cases after the minimal
source-list correction. ARM64 cross-built all 15 C89 cases in addition to the
focused provider/native/package consumers, with execution deferred. Static/DLL
packages passed x86/x64; supplementary MSVC 16 /Od-/O2 consumers and their
87 retained input hashes matched the rebuilt runtime. Source quality passed
375 files and traceability passed 100 mappings before the next tranche was
prepared; import/diff gates passed. Runtime/header/backend growth is about
25 lines; fixtures are about 160 lines plus explicit dependency list updates,
within estimate. Initial zero mask preserves previous defaults. Concurrent
mask ordering, stream creation, variadic _open mode parity and genuine
VC6/reference qualification remain open; actual effort was untimed.
