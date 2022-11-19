[![LICENSE](https://www.gnu.org/graphics/gplv3-127x51.png)](https://www.gnu.org/licenses/gpl-3.0)

Visualizer
==========

Visualizer for a song I made using abstract forms in different colors dancing
after a given choreography.

**This project is still work in progress!**

Build
-----

In order to build this project you need

 - [conan](https://conan.io/)
 - [CMake](ihttps://cmake.org/)
 - [Visual C++](https://visualstudio.microsoft.com/de/vs/)
 - or [GCC](https://gcc.gnu.org/)

The build you need to install the necessary conan packages, run CMake and
compile the project with your compiler:

    > mkdir build && cd build
    > conan install ..
    > cmake ..
    > cmake --build . --config Release

You start the project with:

    > ./bin/visualizer ../choreography.json ../dream.wav

Attributions
------------

 * [https://learnopengl.com/](https://learnopengl.com/)
 * [https://www.warmplace.ru/soft/sunvox/](https://www.warmplace.ru/soft/sunvox/)

Example
-------

![Example](/example.png?raw=true)
