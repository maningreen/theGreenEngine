{
  pkgs ? import <nixpkgs> { },
  ...
}:
pkgs.stdenv.mkDerivation {
  pname = "out";
  version = "0.0.0";
  src = ./.;
  buildInputs = with pkgs; [
    clang
    sol2
    lua
    raylib
    zig
  ];
  nativeBuildInputs = with pkgs; [
    lua
  ];

  buildPhase = ''
    zig build
  '';

  installPhase = ''
    mkdir -p $out/bin
    zig build install -p $out/bin
  '';

  postPatch = ''
    ln -s ${pkgs.callPackage ./deps.nix { }} $ZIG_GLOBAL_CACHE_DIR/p
  '';
}
