#ifndef STL2GCODE_FIXED_H
#define STL2GCODE_FIXED_H

#include <ostream>
#include <cstdint>

class Fixed {
    int64_t fraction;
    static const int16_t exponent;
public:
    Fixed();
    Fixed(const int64_t& fraction);
    Fixed(int i);
    Fixed(float f);
    Fixed(double d);

    Fixed operator-() const ;
    Fixed operator+(const Fixed& fixed) const ;
    Fixed operator-(const Fixed& fixed) const ;
    Fixed operator*(const Fixed& fixed) const ;
    Fixed operator/(const Fixed& fixed) const ;

    Fixed& operator+= (const Fixed& fixed);

    bool operator==(const Fixed& fixed) const ;
    bool operator!=(const Fixed& fixed) const ;
    bool operator<(const Fixed& fixed) const ;
    bool operator>(const Fixed& fixed) const ;
    bool operator<=(const Fixed& fixed) const ;
    bool operator>=(const Fixed& fixed) const ;

    int floor() const ;
    Fixed abs() const ;
    Fixed square() const ;
    Fixed sqrt() const ;

    friend std::ostream& operator<<(std::ostream& stream, const Fixed& fixed);
};

#endif //STL2GCODE_FIXED_H
