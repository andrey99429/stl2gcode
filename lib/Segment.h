#ifndef STL2GCODE_SEGMENT_H
#define STL2GCODE_SEGMENT_H

#include "Vertex.h"
#include <ostream>

class Segment {
public:
    Vertex v1;
    Vertex v2;

    explicit Segment(Vertex v1 = Vertex(), Vertex v2 = Vertex());

    Vertex intersect(const Fixed& z) const ;
    bool contain(const Vertex& v) const ;
    bool operator==(const Segment& s) const ;

    friend std::ostream& operator<<(std::ostream& stream, const Segment& segment);
};


#endif
