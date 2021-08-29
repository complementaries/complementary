# Complementary

## Local development

Setup the build directory with `meson build`, compile with `ninja -C build` and run the program with `./build/complementary`

### Windows (VS 2019)

To setup the environment, open `cmd.exe` and run
```
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64
meson build --backend vs
```
, then open the `.sln` file in the `build` directory in Visual Studio or run `meson compile -C build -j0`.

## Formatting

To install the auto-formatting pre-commit hooks, run `./tools/install-hooks.sh`. To format all files locally, run
`./tools/format.sh`.

## Switch port

Install [devktPro](https://devkitpro.org/wiki/Getting_Started) and run
`dpk-pacman -S switch-dev switch-glad switch-sdl2 switch-sdl2_image switch-sdl2_mixer`, then run `mkdir romfs && cp -r assets romfs/assets && make`
