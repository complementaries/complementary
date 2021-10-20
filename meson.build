project('complementary', 'cpp', default_options : ['cpp_std=c++17'])

src = [
  'src/graphics/gl/Shader.cpp',
  'src/graphics/gl/VertexBuffer.cpp',
  'src/graphics/gl/Texture.cpp',
  'src/graphics/RenderState.cpp',
  'src/graphics/Buffer.cpp',
  'src/graphics/TextureRenderer.cpp',
  'src/graphics/Color.cpp',
  'src/graphics/Window.cpp',
  'src/graphics/Font.cpp',
  'src/imgui/ImGuiUtils.cpp',
  'src/math/Matrix.cpp',
  'src/math/Vector.cpp',
  'src/math/Random.cpp',
  'src/objects/ColorObject.cpp',
  'src/objects/MovingObject.cpp',
  'src/objects/MovingSwitchObject.cpp',
  'src/objects/KeyObject.cpp',
  'src/objects/DoorObject.cpp',
  'src/objects/Object.cpp',
  'src/objects/ObjectRenderer.cpp',
  'src/objects/Objects.cpp',
  'src/objects/TileEditorProp.cpp',
  'src/sound/SoundManager.cpp',
  'src/objects/WindObject.cpp',
  'src/particles/ParticleSystem.cpp',
  'src/player/Ability.cpp',
  'src/player/Face.cpp',
  'src/player/Player.cpp',
  'src/tilemap/tiles/BoxTile.cpp',
  'src/tilemap/tiles/GoalTile.cpp',
  'src/tilemap/tiles/SpawnTile.cpp',
  'src/tilemap/tiles/SpikeTile.cpp',
  'src/tilemap/tiles/Tile.cpp',
  'src/tilemap/Tilemap.cpp',
  'src/tilemap/TilemapEditor.cpp',
  'src/tilemap/Tiles.cpp',
  'src/Game.cpp',
  'src/Clock.cpp',
  'src/Menu.cpp',
  'src/Input.cpp',
  'src/Arguments.cpp',
  'src/AbilityCutscene.cpp',
  'src/Savegame.cpp',
  'src/Main.cpp'
]

zlib_proj = subproject('zlib')
libjpeg_proj = subproject('libjpeg')
libpng_proj = subproject('libpng')
libogg_proj = subproject('libogg')
vorbis_proj = subproject('vorbis')

sdl2_proj = subproject('sdl2')
sdl2_image_proj = subproject('sdl2_image')
sdl2_mixer_proj = subproject('sdl2_mixer')
glew_proj = subproject('glew')
imgui_proj = subproject('imgui')
rapidjson_proj = subproject('rapidjson')
stb_proj = subproject('stb')

dependencies = [
    zlib_proj.get_variable('zlib_dep'),
    libjpeg_proj.get_variable('jpeg_dep'),
    libpng_proj.get_variable('libpng_dep'),
	libogg_proj.get_variable('libogg_dep'),
	vorbis_proj.get_variable('vorbis_dep'),

    sdl2_proj.get_variable('sdl2_dep'),
    sdl2_image_proj.get_variable('sdl2_image_dep'),
	sdl2_mixer_proj.get_variable('sdl2_mixer_dep'),
    glew_proj.get_variable('glew_dep'),
    imgui_proj.get_variable('imgui_dep'),
    rapidjson_proj.get_variable('rapidjson_dep'),
    stb_proj.get_variable('stb_dep')
]

args = ['-Wall']
if host_machine.system() == 'windows'
    args += []
elif host_machine.system() == 'darwin' or host_machine.system() == 'linux'
    args += ['-Werror', '-Wextra', '-pedantic', '-Wno-comment', '-Wno-unused-parameter']
endif

if host_machine.system() == 'linux'
    args += ['-Wno-format-truncation']
endif

if host_machine.system() == 'darwin'
    dependencies += dependency('appleframeworks', modules: 'OpenGL')
endif

exe = executable('complementary', 
    sources: src,
    dependencies: dependencies,
    cpp_args: args,
    include_directories: 'src')
