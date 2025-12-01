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
        sol2 # lib for lua
        lua # lua itself
      ]
      ++ raylibPackages;
    shellHook = ''
      rm compile_commands.json
      echo "Creating compile_commands.json (for clangd)"
      echo "Cleaning..."
      make clean --quiet
      echo "Building..."
      ${pkgs.lib.getExe pkgs.bear} -- make -j $(nproc) --quiet
      echo "compile_commands.json created!"
    '';
  }
