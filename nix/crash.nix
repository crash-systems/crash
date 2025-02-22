{
  stdenv,
  lib,
  withDevBinaries ? false,
}:
stdenv.mkDerivation {
  name = "crash";
  version = "0.1";

  src = ./..;
  makeFlags = ["PREFIX=${placeholder "out"}"];

  buildFlags = ["crash"] ++ (lib.optional withDevBinaries ["debug" "check"]);

  postInstall = lib.optional withDevBinaries ''
    install -Dm755 -t $out/bin debug
    install -Dm755 -t $out/bin check
  '';

  meta = {
    description = "Crash resilient auspicious shell.";
    maintainers = with lib.maintainers; [savalet sigmanificient];
    license = lib.licenses.mit;
    platforms = [lib.platforms.linux "aarch64-darwin"];
    mainProgram = "crash";
  };
}
