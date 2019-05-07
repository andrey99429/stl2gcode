cmake_minimum_required(VERSION 3.12)
project(stl2gcode)

set(CMAKE_CXX_STANDARD 11)

include_directories(stl2gcode/)

add_executable(stl2gcode
        main.cpp

        stl2gcode/stl2gcode.cpp
        stl2gcode/stl2gcode.h
        stl2gcode/Vertex.cpp
        stl2gcode/Vertex.h
        stl2gcode/Triangle.cpp
        stl2gcode/Triangle.h
        stl2gcode/Segment.cpp
        stl2gcode/Segment.h
        stl2gcode/Contour.cpp
        stl2gcode/Contour.h
        stl2gcode/stl2gcode_parameters.h
        )