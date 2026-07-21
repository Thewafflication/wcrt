# C89 API presence tests

These are compile-only tests for REQ-0001 through REQ-0015. They verify that
the public C89 surface exists and has usable declarations; they do not yet
validate runtime behavior.

Compile each translation unit with the WCRT include directory first and without
falling back to host C runtime headers. For TinyCC, the intended form is:

```bat
tcc -std=c89 -nostdinc -I include -c tests\c89\presence\assert.c
```

Repeat for every `.c` file in this directory. Linking is intentionally not
required: assigning function addresses to exact pointer types produces a
compile-time declaration check while allowing implementation work to follow.

Each test includes exactly its owning public header. This catches headers that
only work because another standard header was included first.
