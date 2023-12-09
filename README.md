# Raylib Audio Visualizer

Simple [raylib](https://github.com/raysan5/raylib) based audio visualizer.

To compile it, place `libraylib.a` and `raylib.h` in the repo folder an run
`./build.sh`.

Usage: `rave <music file>`

Options: `[-w<width in pixels>]`, `[-h<height in pixels>]`, `[-f<fps>]`,
`[-b<buffer size in 64 bit words>]`.

Once in the visualizer, use 'q' or 'ESC' to quit gracefully (or force close the
window if you don't mind a segfault).
