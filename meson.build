project('complementary', 'cpp', default_options : ['cpp_std=c++17'])

src = [
  'src/graphics/gl/Shader.cpp',
  'src/graphics/gl/VertexBuffer.cpp',
  'src/graphics/Window.cpp',
  'src/graphics/Buffer.cpp',
  'src/graphics/Color.cpp',
  'src/Game.cpp',
  'src/Input.cpp',
  'src/tilemap/Tilemap.cpp',
  'src/tilemap/Tile.cpp',
  'src/tilemap/Tiles.cpp',
  'src/Main.cpp'
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

args = ['-Wall']
if host_machine.system() == 'windows'
    args += []
elif host_machine.system() == 'darwin' or host_machine.system() == 'linux'
    args += ['-Werror', '-Wextra', '-pedantic']
endif

if host_machine.system() == 'darwin'
    dependencies += dependency('appleframeworks', modules: 'OpenGL')
endif

exe = executable('complementary', 
    sources: src,
    dependencies: dependencies,
    cpp_args: args,
    include_directories: 'src')
