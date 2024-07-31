# abstract-city
Lightweight engine using [Raylib](https://github.com/raysan5/raylib).

## Building
(Currently it only supports windows, Linux porting is work in progress, it should be ready soon)
- This project has submodule dependencies, so make sure to use `git clone --recursive`
- Build normally with `cmake`
- Side note: if you download it without submodules, use `git submodule update`
- Make sure the built binaries are right under `binaries/` and not under `binaries/debug/` as some cmake configurations will force it to be so