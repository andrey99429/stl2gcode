#include <iostream>
#include "lib.h"

using namespace std;

int main() {
    stl2gcode_parameters parameters{};

    string path = "../models/";
    vector<string> models = {"test_model.stl",
                             "3D_test.stl",
                             "pyramid.stl"
                             };

    parameters.nozzle_diameter = 0.4f;
    parameters.thread_thickness = 2.85f;

    Mesh mesh(path + models[0], parameters);
    mesh.stl2gcode();
    mesh.debug_file();

    cout << "python"; auto start = chrono::system_clock::now();
    system("/Users/andrey/OneDrive/Project/stl2gcode/venv/bin/python /Users/andrey/OneDrive/Project/stl2gcode/main.py");
    auto end = chrono::system_clock::now(); cout << ": " << chrono::duration_cast<chrono::milliseconds>(end-start).count() / 1000.0;
    return 0;
}