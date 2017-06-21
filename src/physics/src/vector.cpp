/**
 * @file vector.cpp
 * Definitions for the 2D Vector class
 */

#include "physics/vector.h"
#include <cmath>

namespace physics {

Vector::Vector() : x(), y() {}

Vector::Vector(double x, double y) : x(x), y(y) {}

bool Vector::operator==(Vector rhs) {
	return(x == rhs.x && y == rhs.y);
}

Vector Vector::operator+(Vector rhs) {
	return Vector(x + rhs.x, y + rhs.y);
}

Vector Vector::operator-(Vector rhs) {
	return Vector(x - rhs.x, y - rhs.y);
}

Vector Vector::operator+(double scalar) {
	return Vector(x + scalar, y + scalar);
}

Vector Vector::operator-(double scalar) {
	return Vector(x - scalar, y - scalar);
}

Vector Vector::operator*(double scalar) {
	return Vector(x * scalar, y * scalar);
}

Vector Vector::operator/(double scalar) {
	return Vector(x / scalar, y / scalar);
}

std::ostream& operator<<(
	std::ostream& ostream,
	const Vector& vector
) {
	ostream << "(" << vector.x << ", " << vector.y << ")";
	return ostream;
}

double Vector::dot(Vector rhs) {
	return (x * rhs.x + y * rhs.y);
}

double Vector::magnitude() {
	return sqrt(x*x + y*y);
}

double Vector::distance(Vector other) {
	return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
}

}
