# blocktrace
An attempt to render block world (Minecraft-like) with raytracing

## Building
Requirements: meson, png++, sdl2pp

```sh
meson setup build -Dbuildtype=release
cd build
ninja
```

## Acknowledgements
- [libSDL2pp](https://github.com/libSDL2pp/libSDL2pp) and
[libSDL 2](https://www.libsdl.org/) - currently unused, but will be used for rendering

- [png++](https://www.nongnu.org/pngpp/) - loading and saving PNG images
