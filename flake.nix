{
  description = "An environment for the shwompEngine";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    zig = {
      url = "github:mitchellh/zig-overlay";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      zig,
      ...
    }:
    let
      supportedSystems = [
        "x86_64-linux"
        "x86_64-darwin"
        "aarch64-linux"
        "aarch64-darwin"
      ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;

      zigOverlay = z: final: prev: {
        zig = z;
      };
    in
    {
      devShells = forAllSystems (
        system:
        let
          pkgs = import nixpkgs {
            inherit system;
            overlays = [ (zigOverlay z) ];
          };
          z = zig.packages.${system}.master;
        in
        {
          default = pkgs.callPackage ./shell.nix { inherit pkgs; };
        }
      );

      packages = forAllSystems (
        system:
        let
          pkgs = import nixpkgs {
            inherit system;
            overlays = [ (zigOverlay z) ];
          };
          z = zig.packages.${system}.master;
        in
        {
          default = pkgs.callPackage ./default.nix { inherit pkgs; };
        }
      );
    };
}
