#ifndef STL2GCODE_MESH_H
#define STL2GCODE_MESH_H

#include "Triangle.h"
#include "Segment.h"
#include <vector>
#include <string>
#include <map>

using namespace std;

typedef vector<Vertex> Contour;

class Mesh {
    static const Fixed near_point, near_distance;

    string file;
    vector<Triangle> triangles;
    map<int, vector<Segment>> segments;
    map<int, vector<Contour>> contours;

public:
    explicit Mesh(const string& file);

    void stl_binary();
    void stl_ascii();
    bool is_ascii();
    void debug_file();

    void slicing();

    vector<Contour> contour_construction(const vector<Segment>& segments);
    vector<Contour> plane_construction(const vector<Triangle> & triangles);

};

float str2float(const string& str);

// slicing() vector<Triangle> -> map<int, vector<Segment>>, map<int, vector<Triangle>>
// contour_construction() vector<Segment> -> Contour (vector<Vertex>) // all belong to one z
// plane_construction() vector<Triangle> -> ? (vector<Contour>) // all belong to one z
// supports_construction() vector<Contour> -> vector<Contour> // all belong to one z

ostream& operator<<(ostream& stream, const Contour& contour);

#endif
