#ifndef VECTOR_H
#define VECTOR_H

struct Vector final {
    Vector();
    Vector(float x, float y);

    const float& operator[](int index) const;
    float& operator[](int index);

    operator float*();
    operator const float*() const;

    Vector& operator+=(const Vector& other);
    Vector operator+(const Vector& other) const;

    Vector& operator-=(const Vector& other);
    Vector operator-(const Vector& other) const;

    Vector& operator*=(float f);
    Vector operator*(float f) const;
    Vector& operator*=(const Vector& other);
    Vector operator*(const Vector& other) const;

    Vector& operator/=(float f);
    Vector operator/(float f) const;
    Vector& operator/=(const Vector& other);
    Vector operator/(const Vector& other) const;

    float getLength() const;
    void normalize();
    const float* data() const;
    float* data();

    float x;
    float y;
};

Vector operator*(float f, const Vector& v);

#endif
