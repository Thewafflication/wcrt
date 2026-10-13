# REQ-0082: Microsoft descriptor translation mode

WCRT shall declare `int _setmode(int descriptor, int mode)` in `<io.h>`
without requiring WCRT_POSIX. Supported modes are exactly `_O_TEXT` and
`_O_BINARY` from `<fcntl.h>`. Success returns the previous mode and preserves
errno; repeated selection of the same mode returns that mode.

Text mode translates CR/LF input and LF output through the existing narrow
stream engine; binary mode transfers bytes unchanged. Mode changes affect
the selected descriptor and its corresponding FILE, retaining native handle,
file position, access/append flags, orientation and error/EOF indicators.
Duplicated descriptors initially copy the mode but subsequent mode changes
are independent, despite sharing their native file position.

Invalid descriptors return -1 with EBADF; otherwise unsupported modes return
-1 with EINVAL and leave the descriptor unchanged. Standard descriptors are
supported when their underlying handles are available. No I/O is performed by
the mode change. Callers shall flush output and synchronize input before
changing mode; changing mode with pending pushback or wide conversion state,
or concurrent operations on the same descriptor, is outside this contract.
REQ-0102 adds _fmode defaults for future opens; _setmode remains independent.
Unicode modes, modern invalid-parameter handlers and DOS Ctrl-Z
end-of-file emulation are excluded. Existing text-engine behavior is retained.

## Verification and impact

TC-0082 verifies header presence, prior-mode returns, raw physical output,
translated input, FILE interaction, access/append preservation, position,
duplicate independence, pipes, available standard handles and error partitions.
Owned file fixtures and packaged static/DLL consumers exclude host CRT imports.
TC-0067 is the descriptor regression obligation. No new Windows imports or
runtime layouts are introduced. Microsoft's
[mode reference](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setmode)
is supplementary; genuine VC6 compiler/reference qualification remains Unknown.
