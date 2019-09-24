project('blocktrace', 'cpp', default_options: ['cpp_std=c++17'])

if not meson.get_compiler('cpp').check_header('png++/png.hpp')
    error('Png++ is required to build')
endif

depends = [
    dependency('sdl2pp'),
    dependency('libpng'),
]

if get_option('enable_openmp')
    openmp = dependency('openmp', required: false)
    if not openmp.found()
        warning('Openmp not available, performance may be degraded')
    endif
    depends += [openmp]
endif

if get_option('enable_ofast')
    add_project_arguments(['-Ofast', '-ffast-math'], language: 'cpp')
endif

executable('main', 'main.cpp', dependencies: depends)
