{ pkgs ? import <nixpkgs> {}, ...}: pkgs.mkShell {
  packages = with pkgs; [
    gnumake # build backend
    gcc # c++ compiler package
    ghc # haskell compiler
    haskell-language-server

    # libraries
    raylib # (visual backend)
  ];
}
