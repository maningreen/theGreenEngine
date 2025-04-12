{
  description = "Raylib development environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self , nixpkgs ,... }: let
    system = "x86_64-linux";

    pkgs = nixpkgs.${system}.legacyPackages;

  in {
    devShells."${system}".default = pkgs.callPackage ./shell.nix;

    packages.${system}.default = pkgs.stdenv.mkDerivation {
      name = "engine";
      description = "a game in a custom engine";
      src = ./.; 
      nativeBuildInputs = with pkgs; [
        gcc
        gnumake
      ];

      buildInputs = with pkgs; [
        libGL

        # X11 dependencies
        xorg.libX11
        xorg.libX11.dev
        xorg.libXcursor
        xorg.libXi
        xorg.libXinerama
        xorg.libXrandr

        raylib
      ];

      buildPhase = ''
        mkdir -p build
        make -j5
      '';
      
      installPhase = ''
        mkdir -p $out/bin
        install -t $out/bin build/engine
        cp ${./resources} -r $out/bin
      '';

    };
  };
}
