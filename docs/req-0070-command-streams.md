# REQ-0070 — Microsoft and selected POSIX command streams

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft `_popen`/`_pclose` and POSIX `popen`/`pclose`

## Scope

This requirement provides one-way shell command streams. Direct executable
argument arrays, bidirectional streams, and asynchronous process APIs are
excluded.

## Requirement

- `<stdio.h>` shall always declare `_popen` and `_pclose`; `popen` and `pclose`
  shall be exposed only under `WCRT_POSIX`.
- Read mode shall connect the child's standard output to a readable WCRT
  stream. Write mode shall connect a writable WCRT stream to child input.
- Only the child endpoint shall be inheritable. The parent endpoint shall use
  the shared WCRT stream/descriptor model.
- `pclose` shall close the stream, wait for its child, release the process
  handle, and return the command exit code.
- Invalid commands, modes, streams, capacity, and process creation shall fail
  without leaking owned handles.
- The implementation shall use only Windows 2000-compatible APIs.

## Verification

**Method:** Header isolation, bidirectional data, status, error, and import tests

**References:** TC-0070

## Relationships

- **Depends on:** REQ-0012 and REQ-0067
- **Conflicts with:** Returning a stream without tracked process ownership

## Implementation Record

The Windows process backend creates child-inheritable endpoints, registers the
parent endpoint as a WCRT stream, and tracks its process until close. TC-0070
captures output, feeds input, checks status, and exercises invalid closure.
