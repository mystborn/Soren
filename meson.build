project('soren', 'c')

cc = meson.get_compiler('c')

compiler_args = ['/DSSO_STRING_BUILD', '/DECS_BUILD', '/DSOREN_BUILD', '/DE4C_BUILD', '/DGDS_BUILD', '/DMIST_LOG_BUILD', '/DSSO_STRING_MACROS="soren_std.h"', '/Zc:preprocessor']

include_files = [
    'submodules/GenericDataStructures/include',
    'submodules/MystEcs/include',
    'submodules/sso_string/include',
    'include',
    '../SDL/include',
    '../SDL_ttf/include',
    '../SDL_image/include',
    '../bdwgc/include'
]

inc = include_directories(include_files)

sources = files([
    './submodules/MystEcs/src/ecs_component_flag.c',
    './submodules/MystEcs/src/ecs_component.c',
    './submodules/MystEcs/src/ecs_entity_set.c',
    './submodules/MystEcs/src/ecs_event.c',
    './submodules/MystEcs/src/ecs_messages.c',
    './submodules/MystEcs/src/ecs_system.c',
    './submodules/MystEcs/src/ecs_world.c',
    './submodules/MystEcs/src/ecs.c',
    './submodules/sso_string/src/sso_string.c',
    './src/collisions/soren_colliders_box.c',
    './src/collisions/soren_colliders_circle.c',
    './src/collisions/soren_colliders_line.c',
    './src/collisions/soren_colliders_point.c',
    './src/collisions/soren_colliders_polygon.c',
    './src/collisions/soren_colliders.c',
    './src/collisions/soren_collision_utils.c',
    './src/collisions/soren_collisions.c',
    './src/collisions/soren_spatial_hash.c',
    './src/ecs/soren_scene.c',
    './src/ecs/soren_world_use_collisions.c',
    './src/external/e4c.c',
    './src/external/mist_log.c',
    './src/external/parson.c',
    './src/graphics/sprites/soren_sprite_atlas.c',
    './src/graphics/sprites/soren_sprite_update_mode.c',
    './src/graphics/sprites/soren_sprite.c',
    './src/graphics/text/soren_font_ttf.c',
    './src/graphics/text/soren_font.c',
    './src/graphics/soren_graphics.c',
    './src/graphics/soren_primitives.c',
    './src/input/soren_input.c',
    './src/input/soren_input_actions.c',
    './src/input/soren_input_gamepad.c',
    './src/input/soren_input_mouse_and_keyboard.c',
    './src/resources/soren_json.c',
    './src/resources/soren_resources.c',
    './src/timing/soren_timer.c',
    './src/soren_compass.c',
    './src/soren_enum_parser.c',
    './src/soren_generics.c',
    './src/soren_std.c'
])

sts_sources = files([
    './sts/scenes/level_scene.c',
    './sts/spawners/player_spawner.c',
    './sts/spawners/tuber_spawner.c',
    './sts/systems/anchor_system.c',
    './sts/systems/animation_update_system.c',
    './sts/systems/debug_collider_system.c',
    './sts/systems/draw_system.c',
    './sts/systems/movement_system.c',
    './sts/systems/player_input_system.c',
    './sts/components.c',
    './sts/game_init.c',
    './sts/main.c',
])

sdl = cc.find_library(
    'SDL3',
    dirs: ['C:/Users/chris/source/c/SDL/VisualC/x64/Debug'],
    required: true)

sdl_ttf = cc.find_library(
    'SDL3_ttf',
    dirs: ['C:/Users/chris/source/c/SDL_ttf/VisualC/x64/Debug'],
    required: true)

sdl_image = cc.find_library(
    'SDL3_image',
    dirs: ['C:/Users/chris/source/c/SDL_image/VisualC/x64/Debug'],
    required: true)

cord = cc.find_library(
    'cord',
    dirs: ['C:/Users/chris/source/c/bdwgc/out/x64/Release'],
    required: true)

gc = cc.find_library(
    'gc',
    dirs: ['C:/Users/chris/source/c/bdwgc/out/x64/Release'],
    required: true)

deps = [sdl, sdl_ttf, sdl_image, cord, gc]

soren = static_library(
    'soren',
    sources,
    include_directories: inc,
    name_suffix: 'lib',
    name_prefix: '',
    dependencies: deps,
    c_args: compiler_args
)

soren_shared = shared_library(
    'soren',
    sources,
    include_directories: inc,
    dependencies: deps,
    c_args: compiler_args
)

soren_playground = executable(
    'soren_playground',
    files(['./playground/playground.c', './playground/run.c']),
    include_directories: inc,
    dependencies: deps,
    link_with: [soren_shared],
    c_args: ['/Zc:preprocessor']
)

camera_playground = executable(
    'camera_playground',
    files(['./playground/camera_playground.c', './playground/run.c']),
    include_directories: inc,
    dependencies: deps,
    link_with: [soren_shared],
    c_args: ['/Zc:preprocessor']
)

sts = executable(
    'sts',
    sts_sources,
    include_directories: inc,
    dependencies: deps,
    link_with: [soren_shared],
    c_args: ['/Zc:preprocessor', '/DGDS_EXE']
)

# minimal = executable(
#     'minimal',
#     files(['./playground/minimal.c']),
#     include_directories: include_directories(['../SDL/include']),
#     dependencies: [sdl],
#     c_args: ['/Zc:preprocessor']
# )