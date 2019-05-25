![vapor](https://cdn.discordapp.com/attachments/524077514306355210/580197672825126913/0001.gif)
**Vaportrace** is the successor to Renderdude: Raytracer, with "not doing it myself" multithreading, noise textures, and other interesting changes and whatnots.

# How to use

Run build.bat, and make sure you have **GLM 0.9.8.5**, a math library used by the raytracer, just in a cozy folder next to the stuff, and use the GCC compiler of course. Make sure it can support c++17 and **OpenMP**. Honestly, you should always have the latest version of the compiler.

Feel free to change the options! They follow the structure of:

*vaportrace filename + .extension extension renderWidth renderHeight colorChannels(standard 3) renderSamples(usually 4)*

![shot](https://cdn.discordapp.com/attachments/524077514306355210/580201217993736232/render.png)

# Objects you can render
Currently supported are Spheres, Planes and WIP~ Triangles. In the future Models will be supported in .obj format.

# Ok Uneven, now is this faster?
Haha I don't know. You could help me make it faster though. Or not, maybe buy a threaddestroyer 23000K.

# Credits where its due!
Some of the libraries from Nothings are used. Syoyo's tinyobjloader is there for when I start working on model stuff.
I thank some of the people from the Game Dev League discord's #misc-dev for helping me with some stuff. They are cool.
