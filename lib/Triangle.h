#ifndef STL2GCODE_TRIANGLE_H
#define STL2GCODE_TRIANGLE_H

#include "Vertex.h"

class Triangle {
public:
    Vertex v1;
    Vertex v2;
    Vertex v3;

    explicit Triangle(Vertex v1 = Vertex(), Vertex v2 = Vertex(), Vertex v3 = Vertex());

    Fixed z_min() const ;
    Fixed z_max() const ;

    bool belong_to_plane(Fixed z) const ;
    std::vector<Vertex> intersect(Fixed z) const ;
    friend std::ostream& operator << (std::ostream& stream, const Triangle& t);
};

#endif
