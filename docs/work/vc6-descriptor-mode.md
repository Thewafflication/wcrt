# Microsoft descriptor mode tranche

## Plan and estimate

REQ-0082 fills the roadmap's _setmode gap using the existing stream translation
flag and descriptor lookup. Estimate: 20-30 implementation lines, 100-150
test lines. Dependencies: descriptor table, narrow text engine and duplication.
No reliable elapsed-time estimate or measurement is recorded.

## Design and verification plan

Validate the descriptor then exact supported mode values; return the old
translation flag and change only that flag. Verify physical output and input
bytes, FILE interaction, position, append/access preservation, duplicate
independence, pipes, standard handles and errors. Require native x86/x64,
static/DLL consumers, ARM64 cross-links, TC-0067 regression, source quality,
traceability and Windows 2000 imports. Genuine VC6 remains Unknown; modern
Unicode modes and DOS Ctrl-Z semantics are separate work.

## Closeout

On 2026-10-01 TC-0082 passed native x86/x64 and packaged static/DLL consumers
without host CRT imports. ARM64 source and consumer cross-links passed;
native execution remains Unknown. TC-0067 passed on x86/x64. Source quality
passed for 300 files, traceability passed for 81 requirements and the Windows
2000 x86 import audit passed. The preceding 62-case extension aggregate passed
serially on x86 and x64 before this tranche was added.

Review confirmed that only the translation flag changes; successful lookup
also initializes available standard descriptors through the existing path.
Tests restore standard modes and prove independent duplicate modes while
checking their shared native position. No production failure was found.
Implementation is 19 lines, within estimate; time was not reliably measured.
Genuine VC6 compiler/reference qualification remains Unknown.

The later full aggregate exposed an obsolete TC-0043 absence assertion for
_setmode. The header audit was corrected to check its approved declaration
inventory in both directions, including REQ-0082/0083. This was a test-impact
oversight in the implementation phase, found during aggregate verification;
the runtime behavior tests had passed. Future header additions must review
both presence and controlled-growth inventories.
