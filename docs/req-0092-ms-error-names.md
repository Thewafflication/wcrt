# REQ-0092: Selected Microsoft low-I/O error names

With WCRT_MS_ERRNO selected before first errno.h inclusion, WCRT shall expose
EIO as integer constant expression 5 and EFBIG as integer constant expression
27 without requiring WCRT_POSIX. This selector alone shall not expose the
other POSIX-selected names, types, functions or feature advertisements. Default
headers retain current absence of these macros; WCRT_POSIX retains its existing
values and vocabulary. Repeated inclusion and combined selectors shall work.

errno's process-global storage, lvalue ABI and WCRT_DLL import behavior remain
unchanged. This is additive source vocabulary for existing low-I/O error codes;
it does not qualify a historical thread-local accessor or complete Microsoft
errno table. Genuine VC6 inventory/reference qualification remains Unknown.

## Verification and impact

TC-0092 checks numeric constant expressions, repeatability, strict/POSIX/MS
isolation, DLL errno interaction and EFBIG after a narrow query of a wide native
position without extending the file. Native/package x86/x64, ARM64 cross-links
and supplementary Microsoft /Od-/O2 consumers apply with TC-0003/0050, quality
and traceability. Header opt-in/rebuild is required; no runtime behavior,
imports, binary layout, dependency or supported-target change occurs. Owned
fixtures are closed/removed; estimates/review are in the work log.
