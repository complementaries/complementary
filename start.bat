call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

meson compile -C build -j0

build\complementary.exe --skip-anim

pause