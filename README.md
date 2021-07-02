Setup
=====

Using conan
-----------

1. Create a build directory (easiest way with VS it to Generate - it will fail but
   that is normal).

2. Run conan. For instance: `conan install ..\..\.. -s build_type=Debug`.<br>
   Adjust build_type to `Release` if needed.

3. Generate as usual.<br>
   If cmake does not find packages by itself, use
   `-DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake`

Manual setup
------------

1. Install [libfmt](https://github.com/fmtlib/fmt).
   It is a regular CMake project, the normal
   `mkdir build && cd build && cmake .. && cmake --build .. && cmake --install ..`
   will work. Maybe add a `-DCMAKE_INSTALL_PREFIX` to install it somewhere useful.

2. Add the install path of libfmt to `-DCMAKE_PREFIX_PATH`.
