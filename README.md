![vapor](https://cdn.discordapp.com/attachments/524077514306355210/580197672825126913/0001.gif)
**Vaportrace** is the successor to Renderdude: Raytracer, with "not doing it myself" multithreading, noise textures, and other interesting changes and whatnots.

# How to use

Run build.bat, and make sure you have **GLM 0.9.8.5**, a math library used by the raytracer, just in a cozy folder next to the stuff, and use the GCC compiler of course. Make sure it can support c++17 and **OpenMP**. Honestly, you should always have the latest version of the compiler.

Feel free to change the options.ini file! Make it do all sorts of good stuff. Easy to understand too.

![shot](https://cdn.discordapp.com/attachments/386259864416157697/583780350015045652/render.png)

# Objects you can render
Currently supported are Spheres, Planes, Diks and WIP~ Triangles. In the future Models will be supported in .obj format.

# Credits where its due!
Some of the libraries from Nothings are used. Syoyo's tinyobjloader is there for when I start working on model stuff.
I thank some of the people from the Game Dev League discord's #misc-dev for helping me with some stuff. They are cool.

AAP-Splendor128 and SimpleJPC-16 palettes created by Adigun Polack.

Knoll-Yilluoma dithering algorithm created by Bisqwit. Also present is a soft shadow technique he used in a DOS Raytracer video.
