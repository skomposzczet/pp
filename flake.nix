{
  description = "A very basic flake";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-24.05";
  };
  outputs = { self, nixpkgs }:
    let 
      pkgs = nixpkgs.legacyPackages."x86_64-linux"; 
    in { 
      devShells."x86_64-linux".default = pkgs.mkShell {
        LD_LIBRARY_PATH = "${pkgs.stdenv.cc.cc.lib}/lib";
        venvDir = ".venv";
        packages = with pkgs; [
          clang-tools
          gdb
          mpi
          python312
          python312Packages.venvShellHook
          python312Packages.pip
          python312Packages.numpy
          python312Packages.pyqt6
          python312Packages.matplotlib
        ];
      };
    };
}
