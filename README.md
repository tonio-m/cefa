```
conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cmake --build . && ./bin/piano
```

```
cd build/ && cmake --build . && ./bin/piano
```

```
cd ..
rm -rf build
```


- midi on the piano roll might not work in platforms other than MacOS
