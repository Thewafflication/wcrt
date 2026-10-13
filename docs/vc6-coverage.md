# VC6 coverage working inventory

Baseline: 9a2564c plus the current compatibility work. Updated 2026-10-01.
This inventory guides the 1.1.0 roadmap's VC6 work. It records the repository's
selected contracts and gaps; freezing the licensed VC6 SP header/library
inventory and differential profile remains open. Every genuine VC6 compiler
and reference-runtime qualification entry is currently **Unknown**.

## Evidence boundaries

TinyCC behavior and packaged static/DLL tests establish the named WCRT
requirements. The supplementary Microsoft compiler major 16 test selects
the VC6 x86 C header branch, compiles 24 headers without host-header search,
creates a Microsoft import library, and executes an x86 DLL consumer without
host CRT imports. It exercises mixed stack varargs, 64-bit calls and selected
new interfaces with both /Od and /O2, including selected macro expansion,
assertions, setjmp/longjmp and signal delivery.
Real C89 calls additionally cover small structure returns, floating arguments/
results/out-parameters, parse stopping pointers, sorting/search callbacks and
allocation operations. The optional run retains
`output/test-results/x86/vc6-consumer-results.json` with tool, input and
executable hashes and separate compiler/reference qualification status.
It does not qualify compiler version 12, a VC6 service pack,
the reference MSVCRT, every macro expansion or every calling convention.

ARM64 cross-links establish compilation and linkage only; native execution
remains required. No percentage of VC6 completeness is assigned because the
reference inventory is not frozen. Secure CRT additions are retained
regressions and do not count as VC6-era profile evidence.

## Selected interface coverage

| Family | Selected repository coverage | Controlled evidence | Remaining qualification or scope |
| --- | --- | --- | --- |
| C header foundation | x86 size_t/ptrdiff_t/wchar_t, offsetof, default-cdecl va_list, compiler-spelled 64-bit types | [REQ-0079](req-0079-vc6-consumer-headers.md), TC-0079 | Genuine VC6; other include orders and macro expansions; calling convention/export audit |
| System types | _dev_t/_ino_t/_off_t; opt-in WCRT_MS_TYPES_ALIASES unprefixed names; preserved POSIX widths | [REQ-0087](req-0087-ms-system-types.md), TC-0087 | Genuine VC6 inventory and reference qualification; legacy time selection |
| DLL data and linkage | Imported errno, _fmode and standard stream objects, generated Microsoft x86 import library | REQ-0003/0079, TC-0079 | Release import-library packaging and full helper-symbol inventory |
| Stream/descriptor association | _fileno, _get_osfhandle, _fdopen | REQ-0044/0067 | Inherited handles and reference behavior |
| Native-handle adoption | _open_osfhandle with selected access/translation and exclusive ownership transfer | [REQ-0090](req-0090-ms-native-handle-adoption.md), TC-0090 | Inheritance controls, Unicode/overlapped modes and reference flag/device inventory |
| Basic descriptor I/O | _open, _close, _read, _write, _lseek, _tell, _commit, _isatty | [REQ-0067](req-0067-descriptor-io.md), TC-0067 | Extended open flags, wider resize, inherited descriptors and additional failure injection |
| Descriptor size/position | _lseeki64/_telli64/_filelength/_filelengthi64; physical queries retain stream state | [REQ-0088](req-0088-ms-descriptor-size-position.md), TC-0088 | Reference nonregular behavior and resize qualification |
| Descriptor resize | _chsize signed-32-bit lengths, raw zero-filled extension/truncation and restored physical position | [REQ-0094](req-0094-ms-descriptor-resize.md), TC-0094 | Genuine VC6/reference qualification, wider length variant and concurrent shared-descriptor behavior |
| Physical EOF | _eof compares physical position and size without inspecting or clearing FILE EOF state | [REQ-0089](req-0089-ms-descriptor-eof.md), TC-0089 | Reference nonregular behavior and DOS Ctrl-Z translation |
| Duplication and pipes | _dup, _dup2, _pipe; independent lifetime/shared position | REQ-0067/0074 | Concurrent operations on the same descriptor, inheritance and append atomicity |
| Shared descriptors | _sopen with four deny modes, selected permissions and mutable low-level creation mask | [REQ-0083](req-0083-ms-shared-descriptor.md), TC-0083/0101 | Extended flags, variadic _open mode parity and reference creation semantics |
| Translation mode | _setmode with _O_TEXT/_O_BINARY and independent duplicate modes | [REQ-0082](req-0082-ms-descriptor-mode.md), TC-0082 | DOS Ctrl-Z end-of-file profile and reference defaults |
| Shared streams | _fsopen, _SH_DENYRW/_SH_DENYWR/_SH_DENYRD/_SH_DENYNO | [REQ-0081](req-0081-ms-shared-stream.md), TC-0081 | Full FILE ABI and reference mode grammar |
| File removal/access/permissions | _unlink, _access, _chmod | REQ-0062/0063/0049 | Reference reserved names, wildcard/encoding cases and OS-error state |
| Current/absolute directories | _mkdir, _getcwd, _fullpath | REQ-0064, TC-0064 | Reference null-source behavior, all path partitions and drive-relative state |
| Directory change/removal | _chdir, _rmdir | [REQ-0080](req-0080-ms-directory-change.md), TC-0080 | Wide paths and non-ASCII/reparse aliases; full per-drive history |
| Path components | _makepath, _splitpath, _MAX_* constants | [REQ-0076](req-0076-ms-path-components.md), TC-0076 | Reference lexical edge cases and wide paths |
| Drive queries | _getdrive and _getdrives selected read-only queries | [REQ-0084](req-0084-ms-drive-query.md), TC-0084 | Long paths and reference query errors |
| Drive control | _chdrive/_getdcwd use Windows-provided drive-relative state | [REQ-0085](req-0085-ms-drive-directory.md), TC-0085 | CRT per-drive bookkeeping, inactive-drive history and full reference semantics |
| Case and string helpers | _stricmp/_strnicmp, _strlwr/_strupr/_strrev, _strdup, _strset/_strnset, _memicmp | REQ-0045/0053–0055/0057 | Historical aliases, multibyte/locale inventory and reference behavior |
| 32-bit radix conversion | _itoa/_ltoa/_ultoa | REQ-0056 | Reference invalid-radix behavior and selected historical aliases |
| 64-bit conversion | _i64toa/_ui64toa/_atoi64/_strtoi64/_strtoui64 and _I64_* limits | [REQ-0078](req-0078-ms-integer64.md), TC-0078 | Licensed inventory membership and reference boundary/error behavior |
| 64-bit formatted I/O | I64 integer printf/scanf modifier | [REQ-0077](req-0077-ms-i64-format.md), TC-0077 | Reference malformed formats and complete wide-format profile |
| Bounded formatting | _snprintf/_vsnprintf selected legacy bounds/termination behavior | REQ-0020, TC-0020 | Reference return values and whole format inventory |
| Explicit-width metadata | _stat64/_fstat64, _utime32/_utime64 | REQ-0048/0049 | VC6 legacy layouts and _utime/_futime spellings |
| Legacy metadata source route | Existing x86 aliases retain _stat64; WCRT_VC6_STAT32 selects explicit versioned 36-byte metadata | [REQ-0086](req-0086-ms-stat32-route.md), TC-0086; [ADR-0008](adr-0008-explicit-stat32-source-route.md) | Historical device/time semantics and genuine VC6 layout/reference qualification |
| Legacy file-time source route | WCRT_VC6_UTIME32 selects _utime/_utimbuf spellings backed by _utime32 | [REQ-0091](req-0091-ms-utime32-route.md), TC-0091 | Binary _utime export, generic calendar ABI and reference qualification |
| Descriptor file-time updates | _futime32/_futime64 borrow disk handles; WCRT_VC6_UTIME32 additionally selects _futime | [REQ-0093](req-0093-ms-descriptor-file-times.md), TC-0093 | Binary _futime export and genuine VC6/reference qualification |
| Default time ABI | WCRT time_t/fpos_t retain their shipped 64-bit ABI | REQ-0012/0015/0079 | Generic binary legacy exports and genuine VC6/reference qualification |
| Narrow calendar source route | WCRT_VC6_TIME32 selects x86 time_t and six generic calendar calls through bounded versioned adapters; default binary ABI remains wide | [REQ-0095](req-0095-vc6-calendar-time32.md), TC-0095; [ADR-0009](adr-0009-explicit-time32-source-route.md) | Generic binary legacy exports, full VC6 timezone/DST/error profile and reference qualification; POSIX layout mixing rejected |
| errno model | Process-global WCRT storage; DLL import selector | REQ-0003/0079 | VC6 per-thread accessor/error-number and _doserrno profile |
| Selected MS error names | WCRT_MS_ERRNO exposes EIO/EFBIG without POSIX declarations | [REQ-0092](req-0092-ms-error-names.md), TC-0092 | Full historical error table and thread-local accessor |
| Filename enumeration | Explicit find32/find32i64, _fsize_t and x86 source aliases; failed next retains records | REQ-0096/0100, TC-0096/0100 native/provider/package and selected consumers | Generic binary/reference ABI remains open; concurrent search operations unqualified |
| Descriptor range locking | _locking and five _LK modes use physical ranges with bounded blocking retries | REQ-0098, TC-0098 | Genuine VC6/reference, concurrent position changes and native Windows 2000 qualification |
| Byte-pair copying | _swab copies full pairs and supports exact in-place operation | REQ-0097, TC-0097 | Genuine VC6/reference qualification and partial-overlap behavior |
| Descriptor creation | _creat creates/truncates read/write descriptors with _fmode defaults with deny-none sharing and selected permissions/mask | REQ-0099/0101/0102, TC-0099/0101/0102 | Wide paths and genuine VC6/reference qualification |
| Process creation mask | _umask returns previous read/write bits, ignores other bits and affects new low-level files | REQ-0101, TC-0101 | Concurrent mask/creation ordering, stream creation and genuine VC6/reference qualification |

| Default translation data | Addressable imported int _fmode selects future implicit text/binary opens; explicit modes take precedence | REQ-0102, TC-0102 | Unicode modes, getter/setter aliases, concurrent updates and genuine VC6/reference qualification |

| Bounded sentinel copying | _memccpy and repeatable memory.h route; unsigned sentinel, bounded stops and next-byte pointer | REQ-0103, TC-0103 | Overlap, invalid nonzero buffers and genuine VC6/reference qualification |

## Next work and release gates

Prioritize the generic legacy time ABI, remaining file-time/size helpers and
the licensed reference inventory. Retain explicit status/file-time source
selectors without conflating them with binary legacy exports. Review extended open
flags and descriptor inheritance before declaring the
low-I/O family complete. Maintain Windows 2000 x86 imports and the shipped
WCRT API contracts throughout.

The release gate additionally needs retained VC6 compiler/service-pack and
reference-runtime identities, exact header/library inventories, approved
differences from differential tests, native target evidence, import-library
packaging and release-readiness approval. The working inventory does not
close those gates. Requirements, test specifications and per-tranche work
records remain the detailed source for each selected contract and result.
