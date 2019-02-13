#include <cmath>

#include "extra.h"

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