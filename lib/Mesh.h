#ifndef STL2GCODE_MESH_H
#define STL2GCODE_MESH_H

#include <vector>
#include <string>
#include <map>

#include "Triangle.h"
#include "Segment.h"
#include "Contour.h"

using namespace std;


class Mesh {
    static const Fixed near_point, near_distance;

    string file;
    //vector<Triangle> triangles;
    //map<int, vector<Segment>> segments;
    //map<int, vector<Contour>> contours;

    vector<Triangle> triangles;
    vector<vector<Segment>> segments;
    vector<vector<Contour>> shells; // стенки
    vector<vector<Segment>> infill; // заполнение (внутреннее + основания) ? vector<Segment>


public:
    explicit Mesh(const string& file);

    void stl_binary();
    void stl_ascii();
    bool is_ascii();
    void debug_file();

    void slicing();

    void stl2gcode();

    vector<Contour> contour_construction(const vector<Segment>& segments);
    vector<Contour> plane_construction(const vector<Triangle>& triangles);
    vector<Segment> filling(const vector<Contour>& contours);

};

// slicing() vector<Triangle> -> map<int, vector<Segment>>, map<int, vector<Triangle>>
// contour_construction() vector<Segment> -> Contour (vector<Vertex>) // all belong to one z
// plane_construction() vector<Triangle> -> ? (vector<Contour>) // all belong to one z
// supports_construction() vector<Contour> -> vector<Contour> // all belong to one z

#endif
