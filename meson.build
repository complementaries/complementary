project('complementary', 'cpp')

src = [
    'Main.cpp'
]

zlib_proj = subproject('zlib')
libjpeg_proj = subproject('libjpeg')
libpng_proj = subproject('libpng')

sdl2_proj = subproject('sdl2')
sdl2_image_proj = subproject('sdl2_image')
glew_proj = subproject('glew')
imgui_proj = subproject('imgui')
rapidjson_proj = subproject('rapidjson')
stb_proj = subproject('stb')

dependencies = [
    zlib_proj.get_variable('zlib_dep'),
    libjpeg_proj.get_variable('jpeg_dep'),
    libpng_proj.get_variable('libpng_dep'),

    sdl2_proj.get_variable('sdl2_dep'),
    sdl2_image_proj.get_variable('sdl2_image_dep'),
    glew_proj.get_variable('glew_dep'),
    imgui_proj.get_variable('imgui_dep'),
    rapidjson_proj.get_variable('rapidjson_dep'),
    stb_proj.get_variable('stb_dep')
]

args = ['-Wall', '-pedantic']
if host_machine.system() == 'windows'
    args += ['/std:c++17']
elif host_machine.system() == 'darwin' or host_machine.system() == 'linux'
    args += ['-Werror', '-Wextra', '-std=c++17']
endif

if host_machine.system() == 'darwin'
    dependencies += dependency('appleframeworks', modules: 'OpenGL')
endif

exe = executable('complementary', 
    sources: src,
    dependencies: dependencies,
    cpp_args: args)
