#include <iostream>
#include "lib.h"

using namespace std;

int main() {
    Mesh mesh("../files/pyramid.stl");
    mesh.slicing();
    mesh.debug_file();

    cout << "python" << endl;
    system("/Users/andrey/OneDrive/Project/stl2gcode/venv/bin/python /Users/andrey/OneDrive/Project/stl2gcode/main.py svg");
    return 0;
}