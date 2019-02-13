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

}

void Mesh::slicing() {
    Fixed z_min = min_element(triangles.begin(), triangles.end(), [] (const Triangle& t1, const Triangle& t2) -> bool {
        return t1.z_min() < t2.z_min();
    })->z_min();
    Fixed z_max = max_element(triangles.begin(), triangles.end(), [] (const Triangle& t1, const Triangle& t2) -> bool {
        return t1.z_max() < t2.z_max();
    })->z_max();

    Fixed d(1); // param

    int p_size = ((z_max - z_min) / d).floor();

    cout << "z_min: " << z_min << " z_max: " << z_max << endl;
    cout << "d: " << d << " p_size: " << p_size << endl;
    cout << "triangles: " << triangles.size() << endl;

    // сортировка треугольников
    vector<vector<Triangle>> levels; // список из треугольников
    levels.resize(p_size + 1);

    for (auto &triangle : triangles) {
        int i = 0;
        if (triangle.z_min() < z_min) {
            i = 0;
        } else if (triangle.z_min() > z_max) {
            i = p_size;
        } else {
            i = ((triangle.z_min() - z_min) / d).floor();
        }
        levels[i].push_back(triangle);
    }

    // построение сечений
    map<int, vector<Triangle>> planes;
    segments.resize(p_size + 1);
    vector<Triangle> a;
    for (int i = 0; i <= p_size; ++i) {
        Fixed plane_z = z_min + d * i;
        //if (levels.count(i) == 1) {
        a.insert(a.end(), levels[i].begin(), levels[i].end());
        //}
        auto last = remove_if(a.begin(), a.end(), [&plane_z](const Triangle &t) -> bool {
            return t.z_max() < plane_z;
        });
        a.erase(last, a.end());
        for (auto &t : a) {
            if (t.belong_to_plane(plane_z)) {
                planes[i].push_back(t);
            } else {
                auto seg = t.intersect(plane_z);
                if (seg.size() == 2) {
                    Segment segment(seg[0], seg[1]);
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

    shells.resize(p_size + 1);
    infill.resize(p_size + 1);
    for (int i = 0; i < segments.size(); ++i) {
        auto contours = contour_construction(segments[i]);
        shells[i].insert(shells[i].end(), contours.begin(), contours.end());
    }

    for (int i = 0; i < shells.size(); ++i) {
        auto segments = filling(shells[i]);
        infill[i].insert(infill[i].end(), segments.begin(), segments.end());
    }


    /*for (auto &elem : segments) {
        auto _countures = contour_construction(elem.second);
        contours[elem.first].insert(contours[elem.first].begin(), _countures.begin(), _countures.end());
    }*/

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
    Fixed dx(0.5f);

    // TODO: улучшить алгоритм (запоминание предыдущего отрезка пересечения)
    for (Fixed x = x_min; x <= x_max; x += dx) {
        vector <Vertex> intersections;
        for (auto& contour: contours) {
            Segment l(Vertex(x, y_min, contour[0].z), Vertex(x, y_max, contour[0].z));
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
        intersections.erase(unique(intersections.begin(), intersections.end()), intersections.end());

        // соединение точек в отрезки
        if (intersections.size() > 1) {
            for (int i = 0; i < intersections.size() - 1; i += 2) {
                fillings.emplace_back(intersections[i], intersections[i + 1]);
            }
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