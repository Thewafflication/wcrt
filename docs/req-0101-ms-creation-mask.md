# REQ-0101: Selected Microsoft process creation mask

io.h shall declare int _umask(int permissions). The process mask starts at zero.
Replace it with permissions masked to _S_IREAD|_S_IWRITE, ignore all other bits
including signed high bits, and return the previous stored mask without
changing errno. No error/parameter-handler path is added. A private snapshot
query shall supply the current mask to low-level file creation.

For _sopen/_creat and _open's existing default creation permissions, validate
requested creation modes first, then apply requested & ~mask from one snapshot
before opening. Affect newly created files only. Effective write suppression
sets the existing readonly attribute policy; read suppression does not make
files unreadable on Windows. Returned descriptor access/translation/sharing
flags follow the original requested open flags, independent of creation mask.
Do not change existing attributes, physical content or position when merely
changing the mask. Initial mask zero preserves prior default behavior.

Mask changes and creates are process scoped. Concurrent mask updates or
mask/creation transaction ordering, full _open variadic permission parity and
stream-creation mask effects remain outside this scope. Caller controls mask
restoration. No allocation, host CRT or new Windows import is added.

## Verification and impact

TC-0101 tests previous values, unsupported/negative bits, errno, every read/write
mask and requested-mode combination, new/existing/readonly behavior and initial
descriptor access. Provider controls verify one snapshot per creation after
validation and unchanged existing-file behavior. Native/static/DLL x86/x64,
ARM64 cross-links and supplementary Microsoft calls apply. Update REQ-0083/0099
fixed-zero limitations to default-zero/mutable mask, inventories/manifests,
quality/traceability and rebuilt packages. VC6/reference remains Unknown.
