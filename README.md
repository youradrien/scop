# ft_scop
42 project "Scop" (Vulkan)

## description
### A graphics assignment, for the school 42
In C++11, with access to the whole <stdlib> and the math lib.

Introduction to modern Vulkan API, the goal is to display a simple .obj file with textures and custom shaders.

## how to run
To build on OSX, clone the repo and `make` 
Prerequisites: `SDL2` and `Vulkan` 

SDL2
```bash
brew install SDL2
``` 
and Vulkan
```bash
brew install vulkan-volk vulkan-loader vulkan-headers vulkan-tools (macOS)
```

Then, use it with an obj file and a texture, such as `./scop obj/teapot.obj textures/nyan.bmp`

Move around with keys:

Key | Action
--- | ------
Up / down | Translation along Z axis
Left / right | Translation along X axis
Q / R | Translation along Y axis
W / S | Rotation around X axis
A / D | Rotation around Y axis
T | Switch texture / grey shades
R | Switch texture by triangle / spherical

A few screenshots:

![alt text](https://raw.githubusercontent.com/a)
