# REQ-0073: Worker thread pool

WCRT shall provide a fixed-size worker pool that runs on the selected pthread
surface and therefore remains compatible with Windows 2000. Callers can create
a pool, submit work, wait for the queue and active workers to drain, shut the
pool down, and destroy it.

Shutdown shall reject new work, finish accepted work, wake idle workers, and
join every worker. Repeated shutdown shall succeed. Pool shutdown and waiting
must not be called by a task executing in that same pool.

TC-0073 verifies four workers can block concurrently, release together, drain
512 queued tasks without loss, reject work after shutdown, and shut down
idempotently.
