#include <iostream>
#include <cfloat>
#include <cmath>

#include "Segment.h"

using namespace std;


Segment::Segment(Vertex v0, Vertex v1) : v0(v0), v1(v1) {}

Vertex Segment::intersect_with_plane(const float &z) const {
    Vertex p0 = v0;
    Vertex slope = v1 - v0;
    Vertex v;
    if (slope.z != 0) {
        float t = (z - p0.z) / slope.z;
        v.x = p0.x + t * slope.x;
        v.y = p0.y + t * slope.y;
        v.z = z;
    } else {
        v = Vertex(FLT_MAX, FLT_MAX, FLT_MAX);
    }
    return v;
}

float Segment::distance(const Vertex &v3) const {
    Vertex slope = v1 - v0;
    float x = ((v3.x - v0.x) * slope.y - (v3.y - v0.y) * slope.x);
    float y = ((v3.y - v0.y) * slope.z - (v3.z - v0.z) * slope.y);
    float z = ((v3.x - v0.x) * slope.z - (v3.z - v0.z) * slope.x);
    return (sqrt(x*x + y*y + z*z)) / sqrt(slope.x*slope.x + slope.y*slope.y + slope.z*slope.z);
}

bool Segment::intersect_with_segment(const Segment &s2, Vertex &intersection) const {
    const Segment& s1 = *this;
    // ax + by = c
    struct Line {
        float a;
        float b;
        float c;
    };
    // from x-x0/x1-x0=y-y0/y1-y0
    // to ax + by = c
    Line l1 = {s1.v1.y - s1.v0.y, s1.v0.x - s1.v1.x, s1.v0.x * (s1.v1.y - s1.v0.y) - s1.v0.y * (s1.v1.x - s1.v0.x)};
    Line l2 = {s2.v1.y - s2.v0.y, s2.v0.x - s2.v1.x, s2.v0.x * (s2.v1.y - s2.v0.y) - s2.v0.y * (s2.v1.x - s2.v0.x)};

    float D = l1.a * l2.b - l2.a * l1.b;
    float Dx = l1.c * l2.b - l2.c * l1.b;
    float Dy = l1.a * l2.c - l2.a * l1.c;

    if (D == 0) {
        return false;
    } else {
        intersection.x = Dx / D;
        intersection.y = Dy / D;
        intersection.z = s1.v0.z;

        bool intersected = intersection.between_with_e(s1.v0, s1.v1) && intersection.between_with_e(s2.v0, s2.v1);
        return intersected;
    }

}

bool Segment::operator==(const Segment &s) const {
    return this->v0 == s.v0 && this->v1 == s.v1;
}

ostream& operator<<(ostream &stream, const Segment& segment) {
    stream << segment.v0 << " " << segment.v1;
    return stream;
}
