{ pkgs ? import <nixpkgs> { } }:
pkgs.mkShell rec {
  # Nix Shells do not really differentiate between buildInputs and nativeBuildInputs.
  nativeBuildInputs = with pkgs; [
    # Here goes the compile-time dependencies, including programs.
    pkg-config
  ] ++ buildInputs;
  buildInputs = with pkgs; [
    # Here goes the runtime dependencies.
    raylib
    alsa-lib
    libpulseaudio
    libGLU
    libGL
    glfw
    xorg.libX11
    xorg.libX11.dev
    xorg.libXi
    xorg.libXcursor
    xorg.libXrandr
    xorg.libXinerama
  ];

  LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath (buildInputs ++ pkgs.raylib.buildInputs);
}
