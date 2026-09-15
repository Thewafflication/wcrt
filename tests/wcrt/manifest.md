# WCRT extension test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0073 | [TC-0073](../../docs/tc-0073-thread-pool.tex) | `../posix/thread_pool.c`, `../posix/presence/thread_pool.c` | Concurrent workers, queued work, drain, shutdown, and errors |
| REQ-0074 | [TC-0074](../../docs/tc-0074-thread-safe-streams.tex) | `../posix/stdio_threads.c`, `../posix/stdio_slots.c` | Distinct concurrent streams, failed-open reclamation, exhaustion, pipe rollback, and duplicate ownership |

These interfaces and guarantees are WCRT extensions. Their presence does not
imply a POSIX thread-pool interface, and REQ-0074 does not add C11 per-stream
locking.
