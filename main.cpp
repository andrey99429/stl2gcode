#include <iostream>
#include "stl2gcode.h"
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

    stl2gcode mesh(path + models[0], parameters);
    mesh.convert("../files/model.gcode");
    mesh.debug_file();

    system("/Users/andrey/OneDrive/Project/stl2gcode/venv/bin/python /Users/andrey/OneDrive/Project/stl2gcode/main.py");
    return 0;
}