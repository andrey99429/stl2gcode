#ifndef STL2GCODE_MESH_H
#define STL2GCODE_MESH_H

#include <vector>
#include <string>
#include <map>

#include "Triangle.h"
#include "Segment.h"
#include "Contour.h"

using namespace std;

/// stl2gcode_parameters - поиск сечений
/// \param z_min - мин z
/// \param z_max - мак z
/// \param dz - шаг
struct stl2gcode_parameters {
    float layer_height = 0.2f;
    float nozzle_diameter = 0.25f;
    float thread_thickness = 1.75f;
    float top_bottom_thickness = 0.5f;
    float shell_thickness = 1.0f; // !!!
    float filling_density = 0.15; // %
    int nozzle_temperature = 205;
    int table_temperature = 55;
    int printing_speed = 35;
    int filling_speed = 40;
    int moving_speed = 90;
    int printing_acceleration = 1000;

    int printer_width = 215;
    int printer_depth = 215;
    int printer_height = 300;
};


class Mesh {
    static const float near_point;
    static const float near_distance;

    string file;
    stl2gcode_parameters parameters;
    float x_min, x_max, y_min, y_max, z_min, z_max;

    vector<Triangle> triangles;
    vector<vector<Segment>> segments;
    vector<vector<Contour>> shells; // стенки
    vector<vector<Segment>> infill; // заполнение (внутреннее + основания)
    vector<int> planes;

    void stl_binary();
    void stl_ascii();
    bool is_ascii();
    void slicing(const float& dz);
    void contour_construction(const vector<Segment>& segments, vector<Contour>& contours);
    void filling(const vector<Contour>& contours, vector<Segment>& fillings, const int& level, const bool& is_plane);
    void gcode(const string& path);

    pair<unsigned int, unsigned int> index(const float& x, const float& y, const float& size);

    void contour_construction2(const vector<Segment>& segments, vector<Contour>& contours);
    vector<Contour> plane_construction(const vector<Triangle>& triangles);

public:
    explicit Mesh(const string& file, const stl2gcode_parameters& parameters);

    void stl2gcode();

    void debug_file();
};

#endif
