# blocktrace
An attempt to render block world (Minecraft-like) with raytracing

## Building
Requirements: meson, png++, sdl2pp

```sh
meson setup 'build' -Dbuildtype=release
cd build
ninja
```

## Optimization
You can build this project with the following optimizations:

- Use OpenMP to use multiple threads (enabled by default)
- Use `-Ofast -ffast-math` compiler flags (disabled by default; may cause visual artifacts)

On my laptop the best performance is achieved by Clang with OpenMP and `-Ofast -ffast-math` enabled.
To build the project in this configuration, pass the following parameters to meson:
`CXX=clang++ meson setup 'build' -Dbuildtype=release -Denable_openmp=true -Denable_ofast=true`.
Replace `setup` with `configure` if you have already set up your build directory earlier (or use
another build directory).

## Acknowledgements
- [libSDL2pp](https://github.com/libSDL2pp/libSDL2pp) and
[libSDL 2](https://www.libsdl.org/) - used for rendering

- [png++](https://www.nongnu.org/pngpp/) - loading and saving PNG images
