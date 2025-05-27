{pkgs ? import <nixpkgs> {}, ...}: let 
  name = "out";
in pkgs.stdenv.mkDerivation {
    inherit name;
    description = "A minimalistic game engine";
    src = ./.; 

    nativeBuildInputs = with pkgs; [
      raylib
    ];

    buildPhase = ''
      mkdir -p build
      make -j3
    '';

    installPhase = ''
      mkdir -p $out/bin
      install -t $out/bin build/${name}
   '';
  }
