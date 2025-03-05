{
  description = "Raylib development environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self , nixpkgs ,... }: let
    system = "x86_64-linux";
  in {
    devShells."${system}".default = let
      pkgs = import nixpkgs {
        inherit system;
      };
    in pkgs.mkShell {
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
      ];
    };

    packages.default = let 
      pkgs = import nixpkgs { inherit system ;};
    in{
      name = "engine";
      src = ./src;
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
      ];

      buildPhase = ''
        make clean
        make all
      '';

    };
  };
}
