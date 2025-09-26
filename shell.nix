{pkgs ? import <nixpkgs> {}, ...}: let
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
in
  pkgs.mkShell {
    packages = with pkgs;
      [
        gnumake # build backend
        gcc # c++ compiler package
        ghc # haskell compiler
        haskell-language-server

      ]
      ++ raylibPackages;
    shellHook = ''
      rm compile_commands.json
      make clean
      ${pkgs.lib.getExe pkgs.bear} -- make -j $(nproc)
    '';
  }
