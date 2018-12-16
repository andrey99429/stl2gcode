#include "Segment.h"

#include <cfloat>

using namespace std;

#include <iostream>

Segment::Segment(Vertex v1, Vertex v2) : v1(v1), v2(v2) {}

Vertex Segment::intersect(const Fixed& z) const {
    Vertex p0 = v1;
    Vertex slope = v2 - v1;
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

bool Segment::contain(const Vertex& v) const {
    Vertex slope = v2 - v1;
    Fixed t;
    if (slope.x != 0) {
        t = (v.x - v1.x) / slope.x;
    } else if (slope.y != 0) {
        t = (v.y - v1.y) / slope.y;
    } else if (slope.z != 0) {
        t = (v.z - v1.z) / slope.z;
    } else {
        //error
    }
    bool res = v.between(v1, v2);
    if (res && slope.x != 0) {
        res &= v.x == v1.x + t * slope.x;
    }
    if (res && slope.y != 0) {
        res &= v.y == v1.y + t * slope.y;
    }
    if (res && slope.z != 0) {
        res &= v.z == v1.z + t * slope.z;
    }
    cout << res << endl;
    return res;
}

bool Segment::operator==(const Segment &s) const {
    return this->v1 == s.v1 && this->v2 == s.v2;
}

ostream& operator<<(ostream &stream, const Segment& segment) {
    stream << segment.v1 << endl << segment.v2;
    return stream;
}