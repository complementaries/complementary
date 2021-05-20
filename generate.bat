call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

if exist build\ (
  meson build --wipe --backend vs
) else (
  meson build --backend vs
)

meson compile -C build -j0

build\complementary.exe

pause