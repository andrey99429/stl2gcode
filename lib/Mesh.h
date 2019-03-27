#ifndef STL2GCODE_MESH_H
#define STL2GCODE_MESH_H

#include <vector>
#include <string>
#include <map>
#include <set>

#include "Triangle.h"
#include "Segment.h"
#include "Contour.h"

using namespace std;


struct stl2gcode_parameters {
    float layer_height = 0.2f; //!< Высота слоя (мм).
    float nozzle_diameter = 0.25f; //!< Диаметр экструдера (мм).
    float thread_thickness = 1.75f; //!< Диаметр пластика (мм).
    float top_bottom_thickness = 0.4f; //!< Высота ? (мм).
    float shell_thickness = 1.0f; //!< Толщена стенок (мм). Не будет реализовано в данной версии.
    float filling_density = 0.15; //!< Плотность заполнения (%).
    int nozzle_temperature = 205; //!< Темпиратура экструдера (С).
    int table_temperature = 55; //!< Темпиратура стола (С).
    int printing_speed = 35; //!< Скорость печати (мм/с).
    int filling_speed = 40; //!< Скорость заполнения (мм/с).
    int moving_speed = 90; //!< Скорость перемещения (мм/с).

    int printer_width = 215; //!< Ширина принтера (мм).
    int printer_depth = 215; //!< Длина принтера (мм).
    int printer_height = 300; //!< Высота принтера (мм).
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
    set<int> planes;

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
