#include "Fixed.h"
#include <cmath>
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
    fraction = static_cast<int64_t>(f * exponent) + (static_cast<int64_t>(10 * f * exponent) % 10 >= 5 ? 1 : 0);
}

Fixed Fixed::operator-() const {
    return {-fraction};
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
    return {(fraction * exponent / fixed.fraction) + ((10 * fraction * exponent / fixed.fraction) % 10 >= 5 ? 1 : 0)};
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

Fixed Fixed::abs() const {
    if (*this >= 0) {
        return *this;
    } else {
        return -*this;
    }
}

Fixed Fixed::square() const {
    return *this * *this;
}

Fixed Fixed::sqrt() const {
    return Fixed(::sqrt((float) fraction / exponent));
}

ostream& operator<<(ostream& stream, const Fixed& fixed) {
    stream << fixed.fraction / Fixed::exponent << ".";
    stream.width(4);
    stream.fill('0');
    stream << fixed.fraction % Fixed::exponent;
    return stream;
}