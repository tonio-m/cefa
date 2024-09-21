cmake_minimum_required(VERSION 3.15)
project(Piano)
add_executable(piano src/main.cpp)

find_path(RAYGUI_INCLUDE_DIRS "src/raygui.h")
target_include_directories(piano PRIVATE ${RAYGUI_INCLUDE_DIRS})

find_package(RtMidi REQUIRED)
target_link_libraries(piano RtMidi::rtmidi)

find_package(raylib REQUIRED)
target_link_libraries(piano raylib)

find_library(OpenGL_LIBRARY OpenGL)
find_library(COCOA_LIBRARY Cocoa)
find_library(IOKIT_LIBRARY IOKit)
target_link_libraries(piano ${OpenGL_LIBRARY} ${COCOA_LIBRARY} ${IOKIT_LIBRARY})

set_target_properties(piano PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
