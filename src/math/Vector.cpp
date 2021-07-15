#include "Vector.h"

#include <cmath>

Vector::Vector(float x, float y) : x(x), y(y) {
}

Vector::Vector() : Vector(0.0f, 0.0f) {
}

const float& Vector::operator[](int index) const {
    return data()[index];
}

float& Vector::operator[](int index) {
    return data()[index];
}

Vector::operator float*() {
    return data();
}

Vector::operator const float*() const {
    return data();
}

Vector& Vector::operator+=(const Vector& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector Vector::operator+(const Vector& other) const {
    Vector v = *this;
    v += other;
    return v;
}

Vector& Vector::operator-=(const Vector& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector Vector::operator-(const Vector& other) const {
    Vector v = *this;
    v -= other;
    return v;
}

Vector& Vector::operator*=(float f) {
    x *= f;
    y *= f;
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
    x *= other.x;
    y *= other.y;
    return *this;
}

Vector Vector::operator*(const Vector& other) const {
    Vector v = *this;
    v *= other;
    return v;
}

void Vector::normalize() {
    *this *= 1.0f / sqrtf(x * x + y * y);
}

const float* Vector::data() const {
    return &x;
}

float* Vector::data() {
    return &x;
}
