# *cefa*
A piano roll desktop application for Chord and Scale practice

![cefa logo](https://github.com/user-attachments/assets/9632b436-7876-4347-bf45-d37593f006be)



CEFA is an acronym that stands for Chord Etude For All, and that is also the notes of the second inversion of Fmaj7!
I'm little by little working on this project in my free time, using it to learn more about C++ and it's environment (I come from python) and using the application for my own music practice.

As you can see it's very early in development. If you want to contribute feel free to open a PR, or email me
at contact@mtonio.com if you want to have a chat about it.


## Build Instructions 
midi on the piano roll might not work in platforms other than MacOS
RtMidi which is being used supports other engines, it's just that I've hardcoded it (for now).

```bash
conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cmake --build . && ./bin/piano
```

```bash
cd build/ && cmake --build . && ./bin/piano
```

### Debug Build 

```bash
conan install . --output-folder=build --build=missing -s build_type=Debug
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cmake --build . && lldb bin/piano
# (gdb) run
# (gdb) backtrace
```
