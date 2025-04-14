{ pkgs ? import <nixpkgs> {}, ...}: let 
  # *this is for raylib*
  raylibPackages = with pkgs; [
    libGL

    # X11 dependencies
    xorg.libX11
    xorg.libX11.dev
    xorg.libXcursor
    xorg.libXi
    xorg.libXinerama
    xorg.libXrandr
  ];
in pkgs.mkShell {
    packages = with pkgs; [
      # make
      gnumake
      gcc # gcc
    ] ++ raylibPackages;

  }
