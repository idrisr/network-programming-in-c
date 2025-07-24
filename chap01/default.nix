{ stdenv }:
stdenv.mkDerivation {
  src = ./.;
  name = "unix_list";
  preInstall = "mkdir -p $out/bin";
  makeFlags = [ "BUILD_DIR=$(out)/bin" ];
}
