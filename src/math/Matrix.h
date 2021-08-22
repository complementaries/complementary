#ifndef MATRIX_H
#define MATRIX_H

#include "Vector.h"

class Matrix final {
  public:
    Matrix();

    Matrix& operator*=(const Matrix& other);
    Matrix operator*(const Matrix& other) const;

    Matrix& unit();
    Matrix& scale(const Vector& v);
    Matrix& transform(const Vector& v);

    const float* getData() const;

  private:
    float data[16];
};

#endif