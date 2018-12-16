#include "Fixed.h"
using namespace std;

const int16_t Fixed::exponent = 10000;

Fixed::Fixed() {
    fraction = 0;
}

Fixed::Fixed(const int64_t& fraction) {
    this->fraction = fraction;
}

Fixed::Fixed(int i) {
    fraction = i * exponent;
}

Fixed::Fixed(float f) {
    fraction = static_cast<int64_t>(f * exponent);
}


Fixed Fixed::operator+(const Fixed& fixed) const {
    return {fraction + fixed.fraction};
}

Fixed Fixed::operator-(const Fixed& fixed) const {
    return {fraction - fixed.fraction};
}

Fixed Fixed::operator*(const Fixed& fixed) const {
    return {fraction * fixed.fraction / exponent};
}

Fixed Fixed::operator/(const Fixed& fixed) const {
    return {fraction * exponent / fixed.fraction};
}


bool Fixed::operator==(const Fixed& fixed) const {
    return fraction == fixed.fraction;
}

bool Fixed::operator!=(const Fixed& fixed) const {
    return fraction != fixed.fraction;
}

bool Fixed::operator<(const Fixed& fixed) const {
    return fraction < fixed.fraction;
}

bool Fixed::operator>(const Fixed& fixed) const {
    return fraction > fixed.fraction;
}

bool Fixed::operator<=(const Fixed& fixed) const {
    return fraction <= fixed.fraction;
}

bool Fixed::operator>=(const Fixed& fixed) const {
    return fraction >= fixed.fraction;
}

int Fixed::floor() const {
    return static_cast<int>(fraction / exponent);
}

ostream& operator<<(ostream& stream, const Fixed& fixed) {
    stream.precision(4);
    stream << (float) fixed.fraction / Fixed::exponent;
    return stream;
}
