{
  pkgs ? import <nixpkgs> { },
  ...
}:
pkgs.mkShell {
  packages = with pkgs; [
    clang
    sol2 # lib for lua
    lua # lua itself
    raylib
  ];
}
