{pkgs ? import <nixpkgs> {}, ...}: let 
  name = "out";
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
in pkgs.stdenv.mkDerivation {
    inherit name;
    description = "A minimalistic game engine";
    src = ./.; 

    buildInputs = with pkgs; [
      # gnumake and gcc are in the stdenv :)
    ] ++ raylibPackages;

    buildPhase = ''
      mkdir -p build
      make -j3
    '';

    installPhase = ''
      mkdir -p $out/bin
      install -t $out/bin build/${name}
   '';
  }
