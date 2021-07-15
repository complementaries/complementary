#ifndef VECTOR_H
#define VECTOR_H

class Vector final {
  public:
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

    void normalize();

  private:
    float data[2];
};

Vector operator*(float f, const Vector& v);

#endif