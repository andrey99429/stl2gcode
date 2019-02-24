#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>
#include <list>

#include "Mesh.h"
#include "extra.h"

using namespace std;


const float Mesh::near_point = 0.00002f;
const float Mesh::near_distance = 0.03f;


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

    struct Face {
        Vertex normal;
        Triangle triangle;
        unsigned short attribute;
    } face;

    ifstream f(file, ios::binary);
    char temp[80];
    f.read(temp, 80);

    unsigned int number;
    f.read((char *) &number, 4);

    for (int i = 0; i < number; ++i) {
        f.read((char *) &face, 50);
        triangles.push_back(face.triangle);
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
    out << ":" << endl;
    for (auto& vector : infill) {
        for (auto& segment : vector) {
            out << segment << endl;
        }
    }
    out.close();
}

void Mesh::stl2gcode() {
    // stl2gcode(parameters)
    std::chrono::time_point<std::chrono::system_clock> start, end;

    cout << "triangles: " << triangles.size() << endl;
    // опредееление габоритов модели
    float x_min = triangles.front().x_min();
    float x_max = triangles.front().x_max();
    float y_min = triangles.front().y_min();
    float y_max = triangles.front().y_max();
    float z_min = triangles.front().z_min();
    float z_max = triangles.front().z_max();

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
    cout << "slicing"; start = std::chrono::system_clock::now();

    float dz = 0.25f;
    slicing(z_min, z_max, dz);

    end = std::chrono::system_clock::now(); cout << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() / 1000.0 << endl;

    // объединение отрезков в контуры
    cout << "contour_construction"; start = std::chrono::system_clock::now();

    for (int i = 0; i < segments.size(); ++i) {
        contour_construction(segments[i], shells[i]);
    }

    end = std::chrono::system_clock::now(); cout << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() / 1000.0 << endl;
    // заполнение модели
    cout << "filling"; start = std::chrono::system_clock::now();

    for (int i = 0; i < shells.size(); ++i) {
        filling(shells[i], infill[i]);
    }

    end = std::chrono::system_clock::now(); cout << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() / 1000.0 << endl;

    // синтез g-code

}

void Mesh::slicing(const float& z_min, const float& z_max, const float& dz) {
    auto p_size = static_cast<unsigned int>(roundf((z_max - z_min) / dz) + 1); // TO DO: найти точную формулу
    cout << "(dz: " << dz << " p_size: " << p_size << ")";

    // сортировка треугольников
    typedef vector<Triangle>::iterator Triangle_;
    vector<vector<Triangle_>> levels; // список из треугольников по уровням
    levels.resize(p_size);

    for (auto triangle = triangles.begin(); triangle != triangles.end(); ++triangle) {
        float i = ((triangle->z_min() - z_min) / dz);
        int ii = static_cast<int>(floorf(i) + (floorf(i) == i ? 0 : 1));
        levels[ii].push_back(triangle);
    }

    // построение сечений
    //map<int, vector<Triangle>> planes;
    segments.resize(p_size);
    list<Triangle_> current;
    for (int i = 0; i < p_size; ++i) {
        float plane_z = z_min + dz * i;
        current.remove_if([&plane_z] (const Triangle_ &t) -> bool {
            return t->z_max() < plane_z;
        });
        current.insert(current.end(), levels[i].begin(), levels[i].end());
        for (auto& t : current) {
            if (t->belong_to_plane(plane_z)) {
                //planes[i].push_back(t);
            } else {
                auto points = t->intersect(plane_z);
                if (points.size() == 2) {
                    // убирает повторы отрезков, полученных на стыках треугольником с горизонтальным ребром
                    if (t->z_max() != plane_z || plane_z == z_max) {
                        segments[i].emplace_back(points[0], points[1]);
                    }
                }
            }
        }
    }

    shells.resize(p_size);
    infill.resize(p_size);
}

void Mesh::contour_construction(const std::vector<Segment>& _segments, vector<Contour>& contours) {
    vector<Segment> segments(_segments.begin(), _segments.end());

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
}

void Mesh::filling(const vector<Contour>& contours, vector<Segment>& fillings) {
    float x_min = contours.front().front().x;
    float x_max = contours.front().front().x;
    float y_min = contours.front().front().y;
    float y_max = contours.front().front().y;
    float z = contours.front().front().z;

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

    float dt(5);
    vector<Segment> infill_lines;
    for (float y = y_min; y < y_max; y += dt) { // ↘ low
        float b = y - x_min;
        float x = y_max - b;
        infill_lines.emplace_back(Vertex(x_min, y, z), Vertex(x, y_max, z));
    }

    for (float x = x_min + dt; x < x_max; x += dt) { // ↘ up
        float b = y_min - x;
        float y = x_max + b;
        infill_lines.emplace_back(Vertex(x, y_min, z), Vertex(x_max, y, z));
    }

    for (float x = x_min; x < x_max; x += dt) { // ↗ low
        float b = y_max + x;
        float y = b - x_max;
        infill_lines.emplace_back(Vertex(x, y_max, z), Vertex(x_max, y, z));
    }

    for (float y = y_min + dt; y < y_max; y += dt) { // ↗ up
        float b = y + x_min;
        float x = b - y_min;
        infill_lines.emplace_back(Vertex(x_min, y, z), Vertex(x, y_min, z));
    }

    for (auto& infill_line : infill_lines) {
        vector<Vertex> intersections;
        for (auto& contour: contours) {
            for (int i = 0; i < contour.size() - 1; ++i) {
                Vertex intersection;
                if (infill_line.intersect_with_segment(Segment(contour[i], contour[i + 1]), intersection)) {
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
    vector<Contour> contours;
    contour_construction(segments, contours);
    for (auto& contour : contours) {
        cout << contour << endl;
    }
    cout << endl;
    return contours;
}

pair<unsigned int, unsigned int> index(const float& x, const float& y, const float& x_min, const float& x_max, const float& y_min, const float& y_max, const float& size, const float& nx) {
    auto index_x = static_cast<unsigned int>(floorf((x - x_min) / size));
    if (x == x_max) index_x -= 1;
    auto index_y = static_cast<unsigned int>(floorf((y - y_min) / size));
    if (y == y_max) index_y -= 1;
    return make_pair(index_x, index_y);
}

vector<Contour> Mesh::contour_construction2(const vector<Segment>& _segments, const float& x_min, const float& x_max, const float& y_min, const float& y_max) {
    typedef vector<Segment>::const_iterator Segment_;

    float size = 0.05f;
    int nx = static_cast<int>(ceilf((x_max - x_min) / size));
    int ny = static_cast<int>(ceilf((y_max - y_min) / size));

    map<unsigned long long, vector<vector<Segment_>::iterator>> index_list;

    vector<Segment_> segments;
    for (auto i = _segments.begin(); i != _segments.end(); ++i) {
        segments.push_back(i);
        //index_list[index()].push_back();
    }
    vector<Contour> contours;

    return contours;
}
