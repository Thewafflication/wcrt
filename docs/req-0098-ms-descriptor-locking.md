# REQ-0098: Selected Microsoft descriptor range locking

io.h shall declare int _locking(int descriptor, int mode, long count).
sys/locking.h shall define _LK_UNLCK/LOCK/NBLCK/RLCK/NBRLCK as 0/1/2/3/4.
Locks start at the current signed-64-bit physical position and cover count
bytes, including beyond EOF. RLCK matches LOCK and NBRLCK matches NBLCK.
UNLCK releases exactly a previously locked region; adjacent locks do not merge.
Preserve FILE state, physical position, file size and errno on success.

Validate the descriptor first (EBADF), then mode/negative count (EINVAL).
Require a regular disk file; nonregular objects report EINVAL. Metadata and
position query failures retain their errors. Zero count is a validated no-op.
Native invalid handle maps EBADF, invalid parameter EINVAL, memory failures
ENOMEM, permission/contention/unlocked-region errors EACCES and other errors
EIO. Expose EDEADLOCK=36 in errno.h for exhausted blocking retries.

Nonblocking lock modes make one attempt. Blocking modes retry only native lock
contention, making ten attempts total and nine one-second waits on exhaustion,
then return -1/EDEADLOCK. Success returns zero; all failures return -1.
Unlock never retries. Borrow native handles; do not reopen or close descriptors.
Concurrent changes to the same descriptor's position are outside this scope.

## Verification and impact

TC-0098 covers modes/signatures, independent handle contention/unlock,
beyond-EOF and 64-bit positions, state/size retention, invalid inputs/types and
provider error/retry counts. Rebuild packages, extend public inventory, manifest,
Microsoft consumers and reviewed Windows 2000 imports. Other descriptor ABI is
unchanged. Work record documents design, estimates and historical API evidence.
Genuine VC6/reference and native Windows 2000 qualification remain Unknown.
