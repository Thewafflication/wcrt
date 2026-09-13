# REQ-0072: Read-only file mapping

With `WCRT_POSIX`, `<sys/mman.h>` shall provide `mmap` and `munmap` for
read-only shared or private file views. Zero lengths, fixed mappings, writable
protection, invalid descriptors, and invalid unmaps shall fail. This is a
bounded Windows portability interface, not a complete POSIX mapping model.

TC-0072 verifies declaration isolation, mapped contents, unmapping, and errors.
