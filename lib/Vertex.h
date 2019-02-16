#ifndef STL2GCODE_VERTEX_H
#define STL2GCODE_VERTEX_H

#include "Fixed.h"
#include <ostream>

class Vertex {
public:
    Fixed x;
    Fixed y;
    Fixed z;
    explicit Vertex(Fixed x = 0, Fixed y = 0, Fixed z = 0);
    bool between(const Vertex& v1, const Vertex& v2) const ;
    bool between_with_e(const Vertex &v1, const Vertex &v2) const ;

    Vertex operator+(const Vertex& v) const ;
    Vertex operator-(const Vertex& v) const ;

    Vertex& operator+=(const Vertex& v);

    bool operator==(const Vertex& v) const ;
    bool operator!=(const Vertex& v) const ;

    bool operator<(const Vertex& v) const ;

    Fixed distance(const Vertex& v) const ;

    friend std::ostream& operator << (std::ostream& stream, const Vertex& v);
};

#endif