# *cefa*
CEFA is an interactive application for practicing jazz chord progressions, built with raylib.

![cefa logo](https://github.com/user-attachments/assets/9632b436-7876-4347-bf45-d37593f006be)


CEFA stands for Chord Etude For All, and is also the second inversion of Fmaj7!

The project is in early development, with core features being actively implemented. Contributors are definitely welcome!

I use this application for my own music practice. If you want some feature implemented or have a chat about the project, open an issue or email me at contact@mtonio.com.

## Getting Started
Binaries for the project are present over at Releases, but you might need to compile it yourself depending on your platform.

## Build Instructions 
```bash
conan install . --output-folder=build --build=missing -o "*:shared=False" -s build_type=Release
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DBUILD_SHARED_LIBS=OFF
cmake --build . && bin/piano
```

### Debug Build 
```bash
conan install . --output-folder=build --build=missing -o "*:shared=False" -s build_type=Debug
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DBUILD_SHARED_LIBS=OFF
cmake --build . && lldb bin/piano
# (gdb) run
# (gdb) backtrace
```
