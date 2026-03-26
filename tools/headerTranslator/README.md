# Raylib Translator

This helper program generates a lua api for sol

# Usage

`zig build run -- raylib.h`
or
`translator raylib.h`

To run in silent mode, supply the `-s` or `--silent` flag as the first flag

And prints out the sol api glue necessary, works for any C header.
Does not do enumerations.
