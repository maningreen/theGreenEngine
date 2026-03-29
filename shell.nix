{
  mkShell,
  clang,
  sol2,
  zig,
  libGL,

  libx11,
  libxcursor,
  libxi,
  libxinerama,
  libxrandr,
  ...
}:
mkShell {
  packages = [
    clang
    sol2 # lib for lua
    zig
    libGL

    # X11 dependencies
    libx11
    libx11.dev
    libxcursor
    libxi
    libxinerama
    libxrandr
  ];
}
