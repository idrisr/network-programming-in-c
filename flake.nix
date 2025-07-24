{
  description = "template for math proofs";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/23.11";
    flake-utils.url = "github:numtide/flake-utils";
    pre-commit-hooks.url = "github:cachix/pre-commit-hooks.nix";
    devenv.url = "github:cachix/devenv";
  };

  outputs = inputs@{ nixpkgs, flake-utils, ... }:
    let
      system = flake-utils.lib.system.x86_64-linux;
      pkgs = nixpkgs.legacyPackages.${system};
      hooks = {
        nixfmt.enable = true;
        deadnix.enable = true;
        beautysh.enable = true;
        chktex.enable = true;
        lacheck.enable = true;
        clang-format.enable = true;
      };
    in {
      checks.${system} = {
        pre-commit-check = inputs.pre-commit-hooks.lib.${system}.run {
          src = ./.;
          inherit hooks;
        };
      };

      devShells.${system} = {
        default = inputs.devenv.lib.mkShell {
          inherit inputs pkgs;
          modules = [{
            pre-commit.hooks = hooks;
            packages = with pkgs; [ clang-tools ccls codespell lldb gdb ];
          }];
        };
      };
    };
}
