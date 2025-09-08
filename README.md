# engine game thingy

this is a game engine i have written by hand and for hand /shrug

# required packages

packages :)

# build process (make)

assuming you have all the required packages (those being)
* g++/gcc
* raylib
* make

execute

`make`

and to run

`./build/engine`

# Using Nix

this is a lot easier if you have nix
This project uses nix as the package backend.

### With Flakes

To build:
```bash
nix build
```

To develop:
```bash
nix develop
```

### Without Flakes

To build:
```bash
nix-build
```

To develop:
```bash
nix-shell
```
