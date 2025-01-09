cmake_minimum_required(VERSION 3.15)
project(Piano)

set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(vendor/raygui/projects/CMake)
add_executable(piano src/main.cpp src/piano.cpp src/chord.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(RtMidi REQUIRED)
target_link_libraries(piano RtMidi::rtmidi)

find_package(raylib REQUIRED)
target_link_libraries(piano raylib)

target_link_libraries(piano raygui)

find_library(OpenGL_LIBRARY OpenGL)
find_library(COCOA_LIBRARY Cocoa)
find_library(IOKIT_LIBRARY IOKit)



if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
endif()

target_link_libraries(piano ${OpenGL_LIBRARY} ${COCOA_LIBRARY} ${IOKIT_LIBRARY})
set_target_properties(piano PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LINK_SEARCH_START_STATIC ON
    LINK_SEARCH_END_STATIC ON
)

