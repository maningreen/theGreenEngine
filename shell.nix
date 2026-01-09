{
  pkgs ? import <nixpkgs> { },
  ...
}:
pkgs.mkShell {
  packages = with pkgs; [
    gnumake # build backend
    gcc # c++ compiler package
    ghc # haskell compiler
    sol2 # lib for lua
    lua # lua itself

    libGL

    # X11 dependencies
    xorg.libX11
    xorg.libX11.dev
    xorg.libXcursor
    xorg.libXi
    xorg.libXinerama
    xorg.libXrandr
  ];
}
