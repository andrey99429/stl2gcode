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
    static const float near_point;
    static const float near_distance;

    string file;

    vector<Triangle> triangles;
    vector<vector<Segment>> segments;
    vector<vector<Contour>> shells; // стенки
    vector<vector<Segment>> infill; // заполнение (внутреннее + основания)

public:
    explicit Mesh(const string& file);

    void stl_binary();
    void stl_ascii();
    bool is_ascii();
    void debug_file();

    void stl2gcode();

    void slicing(const float & z_min, const float & z_max, const float & dz);
    void contour_construction(const vector<Segment>& segments, vector<Contour>& contours);
    void filling(const vector<Contour>& contours, vector<Segment>& fillings);


    vector<Contour> contour_construction2(const vector<Segment>& segments, const float& x_min, const float& x_max, const float& y_min, const float& y_max);
    vector<Contour> plane_construction(const vector<Triangle>& triangles);

};

#endif
