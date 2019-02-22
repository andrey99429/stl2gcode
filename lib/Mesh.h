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

    void slicing(const Fixed& z_min, const Fixed& z_max, const Fixed& dz);

    void stl2gcode();

    vector<Contour> contour_construction(const vector<Segment>& segments);
    vector<Contour> contour_construction2(const vector<Segment>& segments,
                                          const Fixed& x_min, const Fixed& x_max,
                                          const Fixed& y_min, const Fixed& y_max);
    vector<Contour> plane_construction(const vector<Triangle>& triangles);
    vector<Segment> filling(const vector<Contour>& contours);

};

#endif
