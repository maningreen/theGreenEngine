{
  mkShell,
  clang,
  sol2,
  raylib,
  zig,
  ...
}:
mkShell {
  packages = [
    clang
    sol2 # lib for lua
    raylib
    zig
  ];
}
