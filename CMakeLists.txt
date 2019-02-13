cmake_minimum_required(VERSION 3.12)
project(stl2gcode)

set(CMAKE_CXX_STANDARD 17)

add_executable(stl2gcode main.cpp lib.h lib/Vertex.cpp lib/Vertex.h lib/Triangle.cpp lib/Triangle.h lib/Segment.cpp lib/Segment.h lib/Mesh.cpp lib/Mesh.h lib/Fixed.cpp lib/Fixed.h lib/Contour.cpp lib/Contour.h lib/extra.h lib/extra.cpp)