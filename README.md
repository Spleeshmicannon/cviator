# Cviator
This is just a personal project where I'm making a modular rendering app that I hope to eventuall turn into a library. This
is written in C, mostly to enforce date oriented design for myself, but also because that makes it easier to make
language bindings.

## Building and running
Provided you have the dependencies: OpenGL 4.6 (or later), GLEW (static), SDL2
You should be able to run the following commands:
```
mkdir build
cd build
cmake .. -G "Unix Makefiles"
make
./cviator
```

This probably won't build very easily on windows. The cmake use the find_package feature which,
in my experience has never worked well on windows. If you want to try and build this on windows
you just need to setup the CMAKE_PREFIX_PATH or point this directly to binaries.

## Software Architecture
Essentially this is setup as a bunch of header files and then a single C file. This is mostly
to improve compile times. I've written other projects where there are a large number of source files
and got tired of how that can blow out compile times. I plan to also package the headers into a
header only library at some point.
