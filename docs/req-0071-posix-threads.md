# REQ-0071: Selected POSIX threads

When `WCRT_POSIX` is selected, WCRT shall provide the mutex,
condition-variable, and thread-lifecycle subset of `<pthread.h>` needed by
concurrent command-line ports using APIs available on Windows 2000.

Static mutex and condition-variable initializers shall initialize safely on
first use. Broadcast shall release every registered waiter. Thread creation,
joining, detaching, explicit exit, and self identification shall be available.
Start-routine and explicit-exit values shall be returned by `pthread_join`.
`pthread_equal` shall compare thread identities, and detached thread resources
shall remain valid until the worker exits.
`pthread_once` shall serialize exactly one successful initialization across
concurrent callers.
Thread-specific keys shall store independent values. For threads created by
`pthread_create`, registered destructors shall run for non-null values at exit,
up to `PTHREAD_DESTRUCTOR_ITERATIONS` passes.

TC-0071 verifies opt-in visibility, static initialization, signaling,
broadcast, joining, cleanup, and invalid arguments.
