let
  pkgs = import (fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-unstable") {};
  build = pkgs.callPackage ./package.nix {};
in
{
  inherit build;
  shell = pkgs.mkShell {
    inputsFrom = [ build ];
    packages = with pkgs; [
      git
      clang-tools
      libclang.python  # for git clang-format
    ];
  };
}
