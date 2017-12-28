/**
 * @file vector.cpp
 * Definitions for the 2D Vector class
 */

#include "physics/vector.h"
#include <cmath>

namespace physics {

Vector::Vector() : x(), y() {}

Vector::Vector(double x, double y) : x(x), y(y) {}

bool Vector::operator==(const Vector &rhs) const {
	return (x == rhs.x && y == rhs.y);
}

bool Vector::operator!=(const Vector &rhs) const {
	return (x != rhs.x || y != rhs.y);
}

Vector Vector::operator+(const Vector &rhs) const {
	return Vector(x + rhs.x, y + rhs.y);
}

Vector Vector::operator-(const Vector &rhs) const {
	return Vector(x - rhs.x, y - rhs.y);
}

Vector Vector::operator+(const double &scalar) const {
	return Vector(x + scalar, y + scalar);
}

Vector Vector::operator-(const double &scalar) const {
	return Vector(x - scalar, y - scalar);
}

Vector Vector::operator*(const double &scalar) const {
	return Vector(x * scalar, y * scalar);
}

Vector Vector::operator/(const double &scalar) const {
	return Vector(x / scalar, y / scalar);
}

std::ostream &operator<<(std::ostream &ostream, const Vector &vector) {
	ostream << "(" << vector.x << ", " << vector.y << ")";
	return ostream;
}

double Vector::dot(const Vector &rhs) const { return (x * rhs.x + y * rhs.y); }

double Vector::magnitude() const { return sqrt(x * x + y * y); }

double Vector::distance(const Vector &other) const {
	return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
}

Vector Vector::floor() const { return Vector(std::floor(x), std::floor(y)); }

Vector Vector::ceil() const { return Vector(std::ceil(x), std::ceil(y)); }
}
