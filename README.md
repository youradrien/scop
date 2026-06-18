# ft_scop
42 project "Scop" (OpenGL)

### A graphics assignment, for the school 42
In C99, with access to the whole stdlib and the math lib.

Introduction to modern OpenGL, the goal is to display a simple .obj file with textures and custom shaders.

To build on OSX, clone the repo and `make` -- prerequisites: GLFW (`brew install glfw`) and GLEW (`brew install glew`)

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
