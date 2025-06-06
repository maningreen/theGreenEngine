{ pkgs ? import <nixpkgs> {}, ...}: pkgs.mkShell {
  packages = with pkgs; [
    gnumake # build backend
    gcc # the compiler used

    # libraries
    raylib # (visual backend)
  ];
}
