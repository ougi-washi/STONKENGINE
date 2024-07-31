# STONKENGINE
Lightweight engine using [Raylib](https://github.com/raysan5/raylib).

## Building
- This project has submodule dependencies, so make sure to use `git clone --recursive`
- Build normally with `cmake`
- Side note: if you download it without submodules, use `git submodule update`
- Make sure the built binaries are right under `bin/` and not under `bin/debug/` or `bin/release/` as some CMake de-kits will force it to be so.
- Currently RayGUI may cause compilations issues for Linux, so it is WIP.