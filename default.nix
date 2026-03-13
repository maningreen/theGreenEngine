{
  pkgs ? import <nixpkgs> { },
  ...
}:
pkgs.stdenv.mkDerivation {
  pname = "out";
  version = "0.0.0";
  src = ./.;
  buildInputs = with pkgs; [
    gnumake # build backend
    gcc # c++ compiler package
    ghc # haskell compiler
    sol2 # lib for lua

    libGL

    # X11 dependencies
    xorg.libX11
    xorg.libX11.dev
    xorg.libXcursor
    xorg.libXi
    xorg.libXinerama
    xorg.libXrandr
  ];
  nativeBuildInputs = with pkgs; [
    lua
  ];

  buildPhase = ''
    make
  '';

  installPhase = ''
    mkdir -p $out/bin
    make install PREFIX=$out/bin -j $(nproc)
  '';
}
