{pkgs ? import <nixpkgs> {}, ...}: pkgs.mkShell {
  packages = with pkgs; [
    libGL

    # X11 dependencies
    xorg.libX11
    xorg.libX11.dev
    xorg.libXcursor
    xorg.libXi
    xorg.libXinerama
    xorg.libXrandr

    # make
    gnumake
    gcc # gcc
    raylib
  ];
}
