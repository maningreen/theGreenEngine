{ pkgs ? import <nixpkgs> {}, ... }: let 
  name = "engine";
in pkgs.stdenv.mkDerivation {
    inherit name;
    description = "A minimalistic game engine";
    src = ./.; 

    nativeBuildInputs = with pkgs; [
      raylib
    ];

    buildPhase = ''
      mkdir -p build
      make -j$(nproc)
    '';

    installPhase = ''
      mkdir -p $out/bin
      cp build/${name} $out/bin
   '';
  }
