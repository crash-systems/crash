{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

    pre-commit-hooks = {
      url = "github:cachix/git-hooks.nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = {
    self,
    nixpkgs,
    pre-commit-hooks,
  }: let
    forAllSystems = function:
      nixpkgs.lib.genAttrs [
        "x86_64-linux"
        "aarch64-linux"
      ] (system: function nixpkgs.legacyPackages.${system});
  in {
    formatter = forAllSystems (pkgs: pkgs.alejandra);

    checks = forAllSystems (pkgs: let
      hooks = import ./nix/hooks.nix {inherit pkgs;};
    in {
      pre-commit-check = pre-commit-hooks.lib.${pkgs.system}.run {
        inherit hooks;
        src = ./.;
      };
    });

    devShells = forAllSystems (pkgs: {
      default = pkgs.mkShell {
        inherit (self.checks.${pkgs.system}.pre-commit-check) shellHook;

        hardeningDisable = ["fortify"];
        packages = with pkgs; [
          gcc
          python3Packages.compiledb
          gcovr
        ];
      };
    });

    packages = forAllSystems (pkgs: {
      default = self.packages.${pkgs.system}.crash;
      crash = pkgs.callPackage ./nix/crash.nix {};
      bundle = pkgs.callPackage ./nix/crash.nix {withDevBinaries = true;};
    });
  };
}
