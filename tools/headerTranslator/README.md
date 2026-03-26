# Raylib Translator

This helper program generates a lua api for sol

# Usage

`zig build run -- raylib.h`
or
`translator raylib.h`

And prints out the sol api glue necessary, works for any C header.
