# REQ-0102: Microsoft default translation-mode data

stdlib.h shall declare process-global extern int _fmode, importing it for
WCRT_DLL consumers. The runtime shall export its addressable storage initialized
to _O_TEXT. Assigning _O_BINARY selects binary defaults for future opens;
assigning _O_TEXT selects text defaults. A caller may read/write it directly.
Variable mutation alone does not change errno, open descriptor modes, stream
state or standard stream translation.

Descriptor _open/_sopen with neither translation flag and _creat shall snapshot
the default once after argument validation and before opening. Stream
fopen/freopen/_fsopen modes lacking t/b shall do likewise after grammar/sharing
validation. Explicit text/binary flags or t/b override any default value.
If an implicit default is neither _O_TEXT nor _O_BINARY, return failure/EINVAL
without opening, creating or truncating. An invalid global does not invalidate
an otherwise valid explicit translation request. Existing modes remain intact
when the global changes; future opens use the new snapshot.

Preserve existing _setmode behavior, creation-mask rules, sharing/ownership and
default text behavior when the variable is unchanged. Binmode.obj, modern
getter/setter aliases, Unicode translation and concurrent mutation/open
transaction ordering remain outside this scope. No new host CRT/Windows import.

## Verification and impact

TC-0102 tests declaration/address/type, initial value, direct writes and DLL
data imports; all implicit/explicit text/binary stream/descriptor paths, physical
CRLF bytes, already-open/standard stream retention, invalid defaults and
provider mutation during opens. Native/static/DLL x86/x64, ARM64 cross-links,
supplementary Microsoft /Od-/O2 and minimal C89/C99 regression builds apply.
Update REQ-0082/0099 limitations, inventories/manifests and explicit source
lists; rebuild packages and retain quality/traceability/import gates. Work record
covers estimates/design/review. Genuine VC6/reference remains Unknown.
