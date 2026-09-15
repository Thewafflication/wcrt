# REQ-0074: Thread-safe stream allocation

WCRT shall allocate, reuse, and release entries of its dynamic stream table so
that callers on different threads each receive a distinct stream. A table
entry shall be claimed before the stream it holds is opened and released only
after that stream is closed, so no caller can be given an entry that another
thread is still preparing or tearing down.

This applies to every operation that claims or releases an entry: `fopen`,
`freopen`, `tmpfile`, `_open`, `open`, `_dup`, `_dup2`, `_pipe`, `pipe`,
`popen`, the internal directory-descriptor and handle-adoption paths, and
release through `fclose`, `_close`, `close`, and `pclose`. A failed open shall
release the entry it claimed. Exhaustion shall report `EMFILE` and shall not
return a shared entry.

The serialization primitive shall be present in every WCRT configuration,
shall not require `WCRT_POSIX` or `<pthread.h>`, and shall use only imports
available on Windows 2000. The representation of `FILE` shall be unchanged, so
the published stream ABI is unaffected.

This requirement does not make one `FILE` object safe for simultaneous use by
more than one thread. WCRT does not provide C11 per-stream locking, and
`flockfile` and `funlockfile` are not supplied. A stream shall be used by one
thread at a time, and callers that share a stream, including the standard
streams, shall serialize access themselves.

TC-0074 verifies that concurrent workers receive distinct streams and
descriptors, that each stream carries only its own data, and that entries are
returned for reuse after closure. The unchanged stream representation remains
covered by the compatibility ABI checks of REQ-0042.

TC-0074 also verifies reclamation after failed `fopen`, `freopen`, and `_open`,
full-table `EMFILE` results without damaging held descriptors, rollback of a
pipe allocation with only one free slot, and reservation and reuse of `_dup`
and `_dup2` targets. These checks use WCRT's bounded dynamic stream table;
they do not claim that arbitrary concurrent use of one descriptor is safe.
