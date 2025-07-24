{
  inputs = {
    nixpkgs.url = "nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        unix_list = pkgs.callPackage ./chap01 { };
      in
      {
        packages.default = unix_list;

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            clang-tools
            ccls
            codespell
            lldb
          ] ++ pkgs.lib.optionals pkgs.stdenv.isLinux [
            gdb
            valgrind
          ];
        };
      });
}
