#include <iostream>
#include "lib.h"

using namespace std;

#include <fstream>

Vertex intersect_lines(const Segment& s1, const Segment& s2) {
    // ax + by = c
    struct Line {
        Fixed a;
        Fixed b;
        Fixed c;
    };

    Line l1 = {s1.v1.y - s1.v0.y, s1.v0.x - s1.v1.x, s1.v0.x * (s1.v1.y - s1.v0.y) - s1.v0.y * (s1.v1.x - s1.v0.x)};
    Line l2 = {s2.v1.y - s2.v0.y, s2.v0.x - s2.v1.x, s2.v0.x * (s2.v1.y - s2.v0.y) - s2.v0.y * (s2.v1.x - s2.v0.x)};

    Fixed D = l1.a * l2.b - l2.a * l1.b;
    Fixed Dx = l1.c * l2.b - l2.c * l1.b;
    Fixed Dy = l1.a * l2.c - l2.a * l1.c;
    Vertex v(Dx/D, Dy/D, 0);
    cout << v << endl;

    return Vertex();
}

void filling() {
    Contour contour1 = {Vertex(103.9231, 90.0000, 0.0000), Vertex(51.9615, 0.0000, 0.0000), Vertex(0.0000, 90.0000, 0.0000), Vertex(103.9231, 90.0000, 0.0000)};
    Contour contour2 = {Vertex(62.5681, 60.0000, 0.0000), Vertex(51.9615, 70.6066, 0.0000), Vertex(41.3549, 60.0000, 0.0000), Vertex(51.9615, 49.3933, 0.0000), Vertex(62.5681, 60.0000, 0.0000)};

    Fixed x_min = min_element(contour1.begin(), contour1.end(), [] (const Vertex& v1, const Vertex& v2) -> bool {
        return v1.x < v2.x;
    })->x;
    Fixed x_max = max_element(contour1.begin(), contour1.end(), [] (const Vertex& v1, const Vertex& v2) -> bool {
        return v1.x < v2.x;
    })->x;
    Fixed y_min = min_element(contour1.begin(), contour1.end(), [] (const Vertex& v1, const Vertex& v2) -> bool {
        return v1.y < v2.y;
    })->y;
    Fixed y_max = max_element(contour1.begin(), contour1.end(), [] (const Vertex& v1, const Vertex& v2) -> bool {
        return v1.y < v2.y;
    })->y;

    vector<Contour> fillings;


    //debug
    ofstream out("../files/model.txt");
    out << contour1 << endl;
    for (auto& segment : fillings) {
        out << segment << endl;
    }
    out.close();
    system("/Users/andrey/OneDrive/Project/stl2gcode/venv/bin/python /Users/andrey/OneDrive/Project/stl2gcode/main.py");
}

int main() {
    /*Mesh mesh("../files/pyramid.stl");
    mesh.slicing();
    mesh.debug_file();

    cout << "python" << endl;
    system("/Users/andrey/OneDrive/Project/stl2gcode/venv/bin/python /Users/andrey/OneDrive/Project/stl2gcode/main.py");*/
    //filling();
    auto v = intersect_lines(Segment(Vertex(2, -2, 0), Vertex(0, 2, 0)), Segment(Vertex(-4, -2, 0), Vertex(4, 0, 0)));
    return 0;
}