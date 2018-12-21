#include "Mesh.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

float str2float(const string& str) {
    size_t pos = str.find('e');
    float res;
    if (pos == string::npos) {
        res = stof(str);
    } else {
        auto fraction = str.substr(0, pos);
        auto exponent = str.substr(pos+1);
        res = static_cast<float>(stof(fraction) * pow(10, stof(exponent)));
    }
    return res;
}

std::vector<Contour> contour_construction(const std::vector<Segment>& _segments) {
    const Fixed near_point(5ll), near_distance(0.3f);
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
            //cout << last << endl;
            auto segment = min_element(segments.begin(), segments.end(), [&last] (const Segment& s1, const Segment& s2) -> bool {
                return min(last.distance(s1.v0), last.distance(s1.v1)) < min(last.distance(s2.v0), last.distance(s2.v1));
            });
            //cout << *segment << endl;
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
        // проверить точки на нахождение на одной прямой
        for (int i = 1; i < contour.size() - 1; ++i) {
            if (contour[i - 1] != contour[i + 1] && Segment(contour[i - 1], contour[i + 1]).distance(contour[i]) <= near_distance) {
                contour.erase(contour.begin() + i);
                --i;
            }
        }
        contours.push_back(contour);
    }

    return contours;
}

ostream& operator<<(ostream& stream, const Contour& countour) {
    for (auto& vertex : countour) {
        stream << vertex << " ";
    }
    return stream;
}

Mesh::Mesh(const std::string &file) {
    this->file = file;

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
    for (auto& vector : contours) {
        for (auto& contour : vector.second) {
            out << contour << endl;
        }
    }
    out.close();
}


void Mesh::slicing() {
    Fixed z_min = min_element(triangles.begin(), triangles.end(), [] (Triangle t1, Triangle t2) -> bool { return t1.z_min() < t2.z_min();})->z_min();
    Fixed z_max = max_element(triangles.begin(), triangles.end(), [] (Triangle t1, Triangle t2) -> bool { return t1.z_max() < t2.z_max();})->z_max();

    float d = 2; // param

    int p_size = ((z_max - z_min) / d).floor();

    cout << "z_min: " << z_min << " z_max: " << z_max << endl;
    cout << "d: "<< d << " p_size: " << p_size << endl;
    cout << "triangles: " << triangles.size() << endl;

    // сортировка треугольников
    map<int, vector<Triangle>> levels; // список из треугольников

    for (auto& triangle : triangles) {
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
    //map<int, vector<Segment>> segments;
    map<int, vector<Triangle>> planes;
    vector<Triangle> a;
    for (int i = 0; i <= p_size; ++i) {
        Fixed plane_z = z_min + d * i;
        if (levels.count(i) == 1) {
            a.insert(a.end(), levels[i].begin(), levels[i].end());
        }
        auto last = remove_if(a.begin(), a.end(), [plane_z] (Triangle t) -> bool { return t.z_max() < plane_z; });
        a.erase(last, a.end());
        for (auto &t : a) {
            if (t.belong_to_plane(plane_z)) {
                planes[i].push_back(t);
            } else {
                auto seg = t.intersect(plane_z);
                if (seg.size() == 2) {
                    Segment segment(seg[0], seg[1]);
                    segments[i].push_back(segment);
                }
            }
        }
    }
    for (auto& elem : segments) {
        auto _countures = contour_construction(elem.second);
        contours[elem.first].insert(contours[elem.first].begin(), _countures.begin(), _countures.end());
    }

    /*cout << "planes" << endl;
    for (auto& v : planes) {
        for (auto& t : v.second) {
            cout << t << endl << endl;
        }
    }*/
}