# REQ-0071: Selected POSIX threads

When `WCRT_POSIX` is selected, WCRT shall provide the mutex,
condition-variable, and thread-lifecycle subset of `<pthread.h>` needed by
concurrent command-line ports using APIs available on Windows 2000.

Static mutex and condition-variable initializers shall initialize safely on
first use. Broadcast shall release every registered waiter. Thread creation,
joining, detaching, explicit exit, and self identification shall be available.
Start-routine return values are not retained; a successful join stores `NULL`
when a result pointer is supplied.

TC-0071 verifies opt-in visibility, static initialization, signaling,
broadcast, joining, cleanup, and invalid arguments.
