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
      pyrun = name: pkgs.python310.interpreter + " " + name;
      hooks = {
        alejandra.enable = true;
        black.enable = true;
        trim-trailing-whitespace.enable = true;

        commit-name = {
          enable = true;
          name = "commit name";
          stages = ["commit-msg"];
          entry = pyrun ./scripts/check_commit_msg.py;
        };

        check-include-order = {
          enable = true;
          name = "commit name";
          entry = pyrun ./scripts/check_include_order.py;
        };
      };
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
  };
}
