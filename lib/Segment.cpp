#include "Segment.h"

#include <cfloat>

using namespace std;

#include <iostream>

Segment::Segment(Vertex v0, Vertex v1) : v0(v0), v1(v1) {}

Vertex Segment::intersect(const Fixed& z) const {
    Vertex p0 = v0;
    Vertex slope = v1 - v0;
    Vertex v;
    if (slope.z != 0) {
        Fixed t = (z - p0.z) / slope.z;
        v.x = p0.x + t * slope.x;
        v.y = p0.y + t * slope.y;
        v.z = z;
    } else {
        v = Vertex(FLT_MAX, FLT_MAX, FLT_MAX);
    }
    return v;
}

Fixed Segment::distance(const Vertex &v3) const {
    Vertex slope = v1 - v0;
    Fixed x = ((v3.x - v0.x) * slope.y - (v3.y - v0.y) * slope.x);
    Fixed y = ((v3.y - v0.y) * slope.z - (v3.z - v0.z) * slope.y);
    Fixed z = ((v3.x - v0.x) * slope.z - (v3.z - v0.z) * slope.x);
    return ((x.square() + y.square() + z.square()).sqrt()) / (slope.x.square() + slope.y.square() + slope.z.square()).sqrt();
}

bool Segment::operator==(const Segment &s) const {
    return this->v0 == s.v0 && this->v1 == s.v1;
}

ostream& operator<<(ostream &stream, const Segment& segment) {
    stream << segment.v0 << " " << segment.v1;
    return stream;
}
