#include <iostream>
#include "lib.h"

using namespace std;

int main() {
    stl2gcode_parameters parameters{};

    string path = "../models/";
    vector<string> models = {"pyramid.stl",
                             "3D_test.stl"};

    Mesh mesh(path + models[1], parameters);
    mesh.stl2gcode();
    mesh.debug_file();

    cout << "python"; auto start = chrono::system_clock::now();
    system("/Users/andrey/OneDrive/Project/stl2gcode/venv/bin/python /Users/andrey/OneDrive/Project/stl2gcode/main.py");
    auto end = chrono::system_clock::now(); cout << ": " << chrono::duration_cast<chrono::milliseconds>(end-start).count() / 1000.0 << endl;
    return 0;
}