#include "Triangle.h"
#include "Segment.h"

#include <algorithm>
#include <vector>
#include <iostream>

using namespace std;

Triangle::Triangle(Vertex v1, Vertex v2, Vertex v3) : v1(v1), v2(v2), v3(v3) {}

Fixed Triangle::x_min() const {
    return min(v1.x, min(v2.x, v3.x));
}

Fixed Triangle::x_max() const {
    return max(v1.x, max(v2.x, v3.x));
}

Fixed Triangle::y_min() const {
    return min(v1.y, min(v2.y, v3.y));
}

Fixed Triangle::y_max() const {
    return max(v1.y, max(v2.y, v3.y));
}

Fixed Triangle::z_min() const {
    return min(v1.z, min(v2.z, v3.z));
}

Fixed Triangle::z_max() const {
    // static Fixed value = max(v1.z, max(v2.z, v3.z));
    // return value;
    return max(v1.z, max(v2.z, v3.z));
}

bool Triangle::belong_to_plane(Fixed z) const {
    return (z == v1.z) && (z == v2.z) && (z == v3.z);
}

std::vector<Vertex> Triangle::intersect(Fixed z) const {
    Segment l1(v1, v2), l2(v2, v3), l3(v3, v1);
    Vertex p1 = l1.intersect_with_plane(z);
    Vertex p2 = l2.intersect_with_plane(z);
    Vertex p3 = l3.intersect_with_plane(z);

    vector<Vertex> res;
    if (p1.between(v1, v2))
        res.push_back(p1);
    if (p2.between(v2, v3) && find(res.begin(), res.end(), p2) == res.end())
        res.push_back(p2);
    if (p3.between(v3, v1) && find(res.begin(), res.end(), p3) == res.end())
        res.push_back(p3);

    return res;
}

std::ostream& operator << (std::ostream& stream, const Triangle& t) {
    stream << t.v1 << " " << t.v2 << " " << t.v3;
    return stream;
}