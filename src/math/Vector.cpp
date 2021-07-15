#include "Vector.h"

#include <cmath>

Vector::Vector(float x, float y) {
    data[0] = x;
    data[1] = y;
}

Vector::Vector() : Vector(0.0f, 0.0f) {
}

const float& Vector::operator[](int index) const {
    return data[index];
}

float& Vector::operator[](int index) {
    return data[index];
}

Vector::operator float*() {
    return data;
}

Vector::operator const float*() const {
    return data;
}

Vector& Vector::operator+=(const Vector& other) {
    data[0] += other.data[0];
    data[1] += other.data[1];
    return *this;
}

Vector Vector::operator+(const Vector& other) const {
    Vector v = *this;
    v += other;
    return v;
}

Vector& Vector::operator-=(const Vector& other) {
    data[0] -= other.data[0];
    data[1] -= other.data[1];
    return *this;
}

Vector Vector::operator-(const Vector& other) const {
    Vector v = *this;
    v -= other;
    return v;
}

Vector& Vector::operator*=(float f) {
    data[0] *= f;
    data[1] *= f;
    return *this;
}

Vector Vector::operator*(float f) const {
    Vector v = *this;
    v *= f;
    return v;
}

Vector operator*(float f, const Vector& v) {
    return v * f;
}

Vector& Vector::operator*=(const Vector& other) {
    data[0] *= other.data[0];
    data[1] *= other.data[1];
    return *this;
}

Vector Vector::operator*(const Vector& other) const {
    Vector v = *this;
    v *= other;
    return v;
}

void Vector::normalize() {
    *this *= 1.0f / sqrtf(data[0] * data[0] + data[1] * data[1]);
}