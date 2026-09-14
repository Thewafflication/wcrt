# WCRT extension test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0073 | [TC-0073](../../docs/tc-0073-thread-pool.tex) | `../posix/thread_pool.c`, `../posix/presence/thread_pool.c` | Concurrent workers, queued work, drain, shutdown, and errors |

These interfaces are WCRT extensions. Their presence does not imply a POSIX
thread-pool interface.
