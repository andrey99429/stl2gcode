#include <algorithm>
#include <iostream>
#include <fstream>

#include "Mesh.h"
#include "extra.h"

using namespace std;


const Fixed Mesh::near_point(5ll);
const Fixed Mesh::near_distance(0.3f);


Mesh::Mesh(const std::string &file) {
    this->file = file;
    fstream f(file);
    if (!f.is_open()) {
        throw invalid_argument("File doesn't exists!");
    }
    f.close();
    if (is_ascii()) {
        stl_ascii();
    } else {
        stl_binary();
    }
}

void Mesh::stl_binary() {
    struct Header {
        char info[80];
        unsigned int number;
    } header;

    struct _Vertex {
        float x;
        float y;
        float z;
    };

    struct Face {
        _Vertex normal;
        _Vertex v1;
        _Vertex v2;
        _Vertex v3;
        unsigned short attribute;
    } face;

    ifstream f(file, ios::binary);
    char temp[81];
    f.get(temp, 81);

    unsigned int number;
    f.get((char *) &number, 5);

    for (int i = 0; i < number; ++i) {
        f.get((char *) &face, 51);

        Triangle triangle;
        triangle.v1.x = Fixed(face.v1.x);
        triangle.v1.y = Fixed(face.v1.y);
        triangle.v1.z = Fixed(face.v1.z);

        triangle.v2.x = Fixed(face.v2.x);
        triangle.v2.y = Fixed(face.v2.y);
        triangle.v2.z = Fixed(face.v2.z);

        triangle.v3.x = Fixed(face.v3.x);
        triangle.v3.y = Fixed(face.v3.y);
        triangle.v3.z = Fixed(face.v3.z);

        triangles.push_back(triangle);
    }
    f.close();
}

void Mesh::stl_ascii() {
    ifstream f(file);
    string line;

    while (!f.eof()) {
        f >> line;
        if (line == "loop") {
            Vertex v1, v2, v3;
            f >> line;
            f >> line;
            v1.x = str2float(line);
            f >> line;
            v1.y = str2float(line);
            f >> line;
            v1.z = str2float(line);

            f >> line;
            f >> line;
            v2.x = str2float(line);
            f >> line;
            v2.y = str2float(line);
            f >> line;
            v2.z = str2float(line);

            f >> line;
            f >> line;
            v3.x = str2float(line);
            f >> line;
            v3.y = str2float(line);
            f >> line;
            v3.z = str2float(line);

            Triangle triangle(v1, v2, v3);
            triangles.push_back(triangle);
        }
    }
    f.close();
}

bool Mesh::is_ascii() {
    ifstream f(file, ios::binary);
    char str[6];
    f.get(str, 6);
    f.close();
    return string(str) == "solid";
}

void Mesh::debug_file() {
    ofstream out("../files/model.txt");
    for (auto& vector : shells) {
        for (auto& contour : vector) {
            out << contour << endl;
        }
    }
    for (auto& vector : infill) {
        for (auto& segment : vector) {
            out << segment << endl;
        }
    }
    out.close();
}

void Mesh::stl2gcode() {
    // stl2gcode(parameters)
    cout << "triangles: " << triangles.size() << endl;
    // опредееление габоритов модели
    Fixed x_min = triangles.front().x_min();
    Fixed x_max = triangles.front().x_max();
    Fixed y_min = triangles.front().y_min();
    Fixed y_max = triangles.front().y_max();
    Fixed z_min = triangles.front().z_min();
    Fixed z_max = triangles.front().z_max();

    for (auto& triangle : triangles) {
        if (x_min > triangle.x_min()) {
            x_min = triangle.x_min();
        }
        if (x_max < triangle.x_max()) {
            x_max = triangle.x_max();
        }
        if (y_min > triangle.y_min()) {
            y_min = triangle.y_min();
        }
        if (y_max < triangle.y_max()) {
            y_max = triangle.y_max();
        }
        if (z_min > triangle.z_min()) {
            z_min = triangle.z_min();
        }
        if (z_max < triangle.z_max()) {
            z_max = triangle.z_max();
        }
    }

    cout << "x_min: " << x_min << " x_max: " << x_max << endl;
    cout << "y_min: " << y_min << " y_max: " << y_max << endl;
    cout << "z_min: " << z_min << " z_max: " << z_max << endl;

    // сдвиг модели в центр
    /*Vertex shift;
    shift.x = -(x_max - x_min) / 2;
    shift.y = -(y_max - y_min) / 2;
    shift.z = -z_min;
    for (auto& triangle: triangles) {
        triangle.v1 += shift;
        triangle.v2 += shift;
        triangle.v3 += shift;
    }*/

    // слайсинг
    Fixed dz(1.0f);
    slicing(z_min, z_max, dz);
}

void Mesh::slicing(const Fixed& z_min, const Fixed& z_max, const Fixed& dz) {
    unsigned int p_size = static_cast<unsigned int>(((z_max - z_min) / dz).floor() + 1);
    cout << "dz: " << dz << " p_size: " << p_size << endl;

    // сортировка треугольников
    vector<vector<Triangle>> levels; // список из треугольников по уровням
    levels.resize(p_size);

    for (auto &triangle : triangles) {
        if (triangle.z_min() <= z_min) {
            levels[0].push_back(triangle);
        }
        else if (triangle.z_min() > z_max) {
            // nothing to do
        } else {
            Fixed i = ((triangle.z_min() - z_min) / dz);
            levels[i.floor() + (i.is_integer() ? 0 : 1)].push_back(triangle);
        }
    }

    // построение сечений
    map<int, vector<Triangle>> planes;
    segments.resize(p_size);
    vector<Triangle> current;
    for (int i = 0; i < p_size; ++i) {
        Fixed plane_z = z_min + dz * i;
        auto last = remove_if(current.begin(), current.end(), [&plane_z](const Triangle &t) -> bool {
            return t.z_max() < plane_z;
        });
        current.erase(last, current.end());
        current.insert(current.end(), levels[i].begin(), levels[i].end());
        for (auto &t : current) {
            if (t.belong_to_plane(plane_z)) {
                planes[i].push_back(t);
            } else {
                auto points = t.intersect(plane_z);
                if (points.size() == 2) {
                    Segment segment(points[0], points[1]);
                    auto repet = find_if(segments[i].begin(), segments[i].end(), [&segment](const Segment &s) -> bool {
                        return (s.v0 == segment.v0 && s.v1 == segment.v1) || (s.v0 == segment.v1 && s.v1 == segment.v0);
                    });
                    if (repet == segments[i].end()) {
                        segments[i].push_back(segment);
                    }
                }
            }
        }
    }

    shells.resize(p_size);
    infill.resize(p_size);
    for (int i = 0; i < segments.size(); ++i) {
        auto contours = contour_construction(segments[i]);
        shells[i].insert(shells[i].end(), contours.begin(), contours.end());
    }

    for (int i = 0; i < shells.size(); ++i) {
        auto segments = filling(shells[i]);
        infill[i].insert(infill[i].end(), segments.begin(), segments.end());
    }

    /*for (auto &v : planes) {
        auto _plane = plane_construction(v.second);
    }*/
}

std::vector<Contour> Mesh::contour_construction(const std::vector<Segment>& _segments) {
    vector<Segment> segments(_segments.begin(), _segments.end());
    vector<Contour> contours;

    while (!segments.empty()) {
        Vertex v1 = segments.begin()->v0;
        Vertex v2 = segments.begin()->v1;
        segments.erase(segments.begin());

        Contour contour;
        contour.push_back(v1);
        contour.push_back(v2);

        while (contour.back().distance(contour.front()) > near_point && !segments.empty()) {
            auto& last = contour.back();
            auto segment = min_element(segments.begin(), segments.end(), [&last] (const Segment& s1, const Segment& s2) -> bool {
                return min(last.distance(s1.v0), last.distance(s1.v1)) < min(last.distance(s2.v0), last.distance(s2.v1));
            });
            if (segment != segments.end() && min(last.distance(segment->v0), last.distance(segment->v1)) <= near_point) {
                if (last.distance(segment->v0) < last.distance(segment->v1)) {
                    contour.push_back(segment->v1);
                } else {
                    contour.push_back(segment->v0);
                }
                segments.erase(segment);
            } else {
                break;
            }
        }
        // что-бы контур правиильно замкнулся
        if (contour.back() != contour.front() && contour.back().distance(contour.front()) <= near_point) {
            contour.back() = contour.front();
        }

        // проверить точки на нахождение на одной прямой
        for (int i = 1; i < contour.size() - 1; ++i) {
            if (contour[i - 1] != contour[i + 1] && Segment(contour[i - 1], contour[i + 1]).distance(contour[i]) <= near_distance) {
                contour.erase(contour.begin() + i);
                --i;
            }
        }
        // проверить является первый или последний лишним
        if (contour.front() == contour.back() && contour.size() > 3) {
            if (Segment(*(contour.begin() + 1), *(contour.end() - 2)).distance(contour.front()) <= near_distance) {
                contour.erase(contour.begin());
                contour.erase(contour.end() - 1);
                contour.push_back(contour.front());
            }
        }
        contours.push_back(contour);
    }

    return contours;
}

vector<Segment> Mesh::filling(const vector<Contour>& contours) {
    Fixed x_min = contours.front().front().x;
    Fixed x_max = contours.front().front().x;
    Fixed y_min = contours.front().front().y;
    Fixed y_max = contours.front().front().y;
    Fixed z = contours.front().front().z;

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

    vector<Segment> fillings;
    Fixed dx(1.0f);

    for (Fixed x = x_min; x <= x_max; x += dx) {
        vector<Vertex> intersections;
        Segment l(Vertex(x, y_min, z), Vertex(x, y_max, z));
        for (auto& contour: contours) {
            for (int i = 0; i < contour.size() - 1; ++i) {
                Vertex intersection;
                if (l.intersect_with_segment(Segment(contour[i], contour[i + 1]), intersection)) {
                    intersections.push_back(intersection);
                }
            }
        }

        // обработка точек
        sort(intersections.begin(), intersections.end(), [] (const Vertex& v1, const Vertex& v2) -> bool {
            return v1.y < v2.y;
        });
        //intersections.erase(unique(intersections.begin(), intersections.end()), intersections.end());

        // соединение точек в отрезки
        if (intersections.size() > 1) {
            for (int i = 0; i < intersections.size() - 1; i += 2) {
                if (intersections[i] != intersections[i + 1]) {
                    fillings.emplace_back(intersections[i], intersections[i + 1]);
                }
            }
        }
    }

    return fillings;
}

vector<Contour> Mesh::plane_construction(const vector<Triangle>& triangles) {
    vector<Segment> segments;
    for (auto& triangle : triangles) {
        segments.emplace_back(triangle.v1, triangle.v2);
        segments.emplace_back(triangle.v2, triangle.v3);
        segments.emplace_back(triangle.v3, triangle.v1);
    }
    cout << segments.size() << endl;
    /*for (auto& segment : segments) {
        cout << segment << endl;
    }*/
    for (auto it = segments.begin(); it < segments.end(); ++it) {
        auto repet = find_if(it+1, segments.end(), [&it] (const Segment& s) -> bool {
            return (it->v0.distance(s.v0) <= near_distance || it->v0.distance(s.v1) <= near_distance) && (it->v1.distance(s.v0) <= near_distance || it->v1.distance(s.v1) <= near_distance);
        });
        if (repet != segments.end()) {
            segments.erase(repet);
            segments.erase(it);
            --it;
        }
    }
    cout << segments.size() << endl;
    /*for (auto& segment : segments) {
        cout << segment << endl;
    }*/
    auto contours = contour_construction(segments);
    for (auto& contour : contours) {
        cout << contour << endl;
    }
    cout << endl;
    return contours;
}