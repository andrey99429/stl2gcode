cmake_minimum_required(VERSION 3.12)
project(stl2gcode)

set(CMAKE_CXX_STANDARD 11)

include_directories(lib/)

add_executable(stl2gcode
        main.cpp

        lib/stl2gcode.cpp
        lib/stl2gcode.h
        lib/Vertex.cpp
        lib/Vertex.h
        lib/Triangle.cpp
        lib/Triangle.h
        lib/Segment.cpp
        lib/Segment.h
        lib/Contour.cpp
        lib/Contour.h
        lib/extra.h
        lib/extra.cpp
        lib/stl2gcode_parameters.h)