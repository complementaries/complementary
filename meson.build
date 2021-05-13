project('complementary', 'cpp')

src = [
    'Main.cpp'
]

sdl2_proj = subproject('sdl2')
sdl2_image_proj = subproject('sdl2_image')
glew_proj = subproject('glew')
imgui_proj = subproject('imgui')
rapidjson_proj = subproject('rapidjson')
stb_proj = subproject('stb')

dependencies = [
    sdl2_proj.get_variable('sdl2_dep'),
    sdl2_image_proj.get_variable('sdl2_image_dep'),
    glew_proj.get_variable('glew_dep'),
    imgui_proj.get_variable('imgui_dep'),
    rapidjson_proj.get_variable('rapidjson_dep'),
    stb_proj.get_variable('stb_dep')
]

args = ['-Wall', '-std=c++17', '-pedantic']
if host_machine.system() == 'linux'
    args += ['-Werror', '-Wextra']
elif host_machine.system() == 'windows'
    if not compiler.compiles(code, name : '#include <EGL/egl.h>')
        args += ['-D_MSC_VER']
    endif
elif host_machine.system() == 'darwin'
    args += ['-Werror', '-Wextra']
    dependencies += dependency('appleframeworks', modules: 'OpenGL')
endif

exe = executable('complementary', 
    sources: src,
    dependencies: dependencies,
    cpp_args: args)
