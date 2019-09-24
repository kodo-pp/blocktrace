project('blocktrace', 'cpp', default_options: ['cpp_std=c++17'])

if not meson.get_compiler('cpp').check_header('png++/png.hpp')
    error('Png++ is required to build')
endif

executable('main', 'main.cpp', dependencies: [dependency('sdl2pp'), dependency('libpng')])
