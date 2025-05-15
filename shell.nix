{ pkgs ? import <nixpkgs> {}, ...}: 
pkgs.mkShell {
  # most useless ass shell.nix
  packages = with pkgs; [
    raylib
  ];
}
