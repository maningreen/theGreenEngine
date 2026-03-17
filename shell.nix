{
  mkShell,
  clang,
  sol2,
  lua,
  raylib,
  zig,
  ...
}:
mkShell {
  packages = [
    clang
    sol2 # lib for lua
    lua # lua itself
    raylib
    zig
  ];
}
