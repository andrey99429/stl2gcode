#include <iostream>
#include "lib.h"

using namespace std;

#include <fstream>

bool intersect_lines(const Segment& s1, const Segment& s2, Vertex& intersection) {
    // ax + by = c
    struct Line {
        Fixed a;
        Fixed b;
        Fixed c;
    };
    // from x-x0/x1-x0=y-y0/y1-y0
    // to ax + by = c
    Line l1 = {s1.v1.y - s1.v0.y, s1.v0.x - s1.v1.x, s1.v0.x * (s1.v1.y - s1.v0.y) - s1.v0.y * (s1.v1.x - s1.v0.x)};
    Line l2 = {s2.v1.y - s2.v0.y, s2.v0.x - s2.v1.x, s2.v0.x * (s2.v1.y - s2.v0.y) - s2.v0.y * (s2.v1.x - s2.v0.x)};

    Fixed D = l1.a * l2.b - l2.a * l1.b;
    Fixed Dx = l1.c * l2.b - l2.c * l1.b;
    Fixed Dy = l1.a * l2.c - l2.a * l1.c;

    if (D == 0) {
        return false;
    } else {
        intersection.x = Dx / D;
        intersection.y = Dy / D;
        intersection.z = s1.v0.z;

        return intersection.between(s1.v0, s1.v1) && intersection.between(s2.v0, s2.v1);
    }

}

void filling() {
    Contour contour1 = {Vertex(103.9231, 90.0000, 0.0000), Vertex(51.9615, 0.0000, 0.0000), Vertex(0.0000, 90.0000, 0.0000), Vertex(103.9231, 90.0000, 0.0000)};
    Contour contour2 = {Vertex(62.5681, 60.0000, 0.0000), Vertex(51.9615, 70.6066, 0.0000), Vertex(41.3549, 60.0000, 0.0000), Vertex(51.9615, 49.3933, 0.0000), Vertex(62.5681, 60.0000, 0.0000)};

    vector<Contour> contours = {contour1, contour2};
    Fixed x_min = contours.front().front().x;
    Fixed x_max = contours.front().front().x;
    Fixed y_min = contours.front().front().y;
    Fixed y_max = contours.front().front().y;

    for (auto& contour: contours) {
        for (auto& vertex: contour) {
            if (vertex.x < x_min) {
                x_min = vertex.x;
            } else if (x_max < vertex.x) {
                x_max = vertex.x;
            }
            if (vertex.y < y_min) {
                y_min = vertex.y;
            } else if (y_max < vertex.y) {
                y_max = vertex.y;
            }
        }
    }

    vector<Contour> fillings;
    Fixed dx(0.5f);

    // TODO: улучшить алгоритм (запоминание предыдущего отрезка пересечения)
    for (Fixed x = x_min; x <= x_max; x += dx) {
        vector <Vertex> intersections;
        for (auto& contour: contours) {
            Segment l(Vertex(x, y_min, contour[0].z), Vertex(x, y_max, contour[0].z));
            for (int i = 0; i < contour.size() - 1; ++i) {
                Vertex intersection;
                if (intersect_lines(l, Segment(contour[i], contour[i + 1]), intersection)) {
                    intersections.push_back(intersection);
                }
            }
        }

        // обработка точек
        sort(intersections.begin(), intersections.end(), [] (const Vertex& v1, const Vertex& v2) -> bool {
            return v1.y < v2.y;
        });
        intersections.erase(unique(intersections.begin(), intersections.end()), intersections.end());

        // соединение точек в отрезки
        for (int i = 0; i < intersections.size() - 1; i += 2) {
            Contour c;
            c.push_back(intersections[i]);
            c.push_back(intersections[i + 1]);
            fillings.push_back(c);
        }
    }

    /*int si1 = 0; // индекс начала первого отрезка имеющего пересечение в прошлый раз
    int si2 = 0;
    for (Fixed x = x_min; x <= x_max; x += dx) {
        Segment l(Vertex(x, y_min, contour1[0].z), Vertex(x, y_max, contour1[0].z));
        Vertex v1, v2;

        while (!intersect_lines(l, Segment(contour1[si1], contour1[si1 + 1]), v1)) {
            si1 += 1;
            if (si1 == contour1.size() - 1) {
                si1 = 0;
            }
        }

        while (!intersect_lines(l, Segment(contour1[si2], contour1[si2 + 1]), v2) || si1 == si2) {
            si2 += 1;
            if (si2 == contour1.size() - 1) {
                si2 = 0;
            }
        }

        // чтобы не получалось вырожденного отрезка
        if (v1 != v2) {
            Contour c;
            c.push_back(v1);
            c.push_back(v2);
            fillings.push_back(c);
        }
    }*/

    //debug
    ofstream out("../files/model.txt");
    for (auto& contour: contours) {
        out << contour << endl;
    }
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
    filling();
    return 0;
}