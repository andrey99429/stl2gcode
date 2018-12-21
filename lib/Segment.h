#ifndef STL2GCODE_SEGMENT_H
#define STL2GCODE_SEGMENT_H

#include "Vertex.h"
#include <ostream>

class Segment {
public:
    Vertex v0;
    Vertex v1;

    explicit Segment(Vertex v0 = Vertex(), Vertex v1 = Vertex());

    Vertex intersect(const Fixed& z) const ;
    Fixed distance(const Vertex& v3) const ;
    bool operator==(const Segment& s) const ;

    friend std::ostream& operator<<(std::ostream& stream, const Segment& segment);
};


#endif
