#include "Matrix.h"

Matrix::Matrix() {
    unit();
}

Matrix& Matrix::operator*=(const Matrix& other) {
    Matrix m;
    m.data[0] = data[0] * other.data[0] + data[4] * other.data[1] + data[8] * other.data[2] +
                data[12] * other.data[3];
    m.data[1] = data[1] * other.data[0] + data[5] * other.data[1] + data[9] * other.data[2] +
                data[13] * other.data[3];
    m.data[2] = data[2] * other.data[0] + data[6] * other.data[1] + data[10] * other.data[2] +
                data[14] * other.data[3];
    m.data[3] = data[3] * other.data[0] + data[7] * other.data[1] + data[11] * other.data[2] +
                data[15] * other.data[3];

    m.data[4] = data[0] * other.data[4] + data[4] * other.data[5] + data[8] * other.data[6] +
                data[12] * other.data[7];
    m.data[5] = data[1] * other.data[4] + data[5] * other.data[5] + data[9] * other.data[6] +
                data[13] * other.data[7];
    m.data[6] = data[2] * other.data[4] + data[6] * other.data[5] + data[10] * other.data[6] +
                data[14] * other.data[7];
    m.data[7] = data[3] * other.data[4] + data[7] * other.data[5] + data[11] * other.data[6] +
                data[15] * other.data[7];

    m.data[8] = data[0] * other.data[8] + data[4] * other.data[9] + data[8] * other.data[10] +
                data[12] * other.data[11];
    m.data[9] = data[1] * other.data[8] + data[5] * other.data[9] + data[9] * other.data[10] +
                data[13] * other.data[11];
    m.data[10] = data[2] * other.data[8] + data[6] * other.data[9] + data[10] * other.data[10] +
                 data[14] * other.data[11];
    m.data[11] = data[3] * other.data[8] + data[7] * other.data[9] + data[11] * other.data[10] +
                 data[15] * other.data[11];

    m.data[12] = data[0] * other.data[12] + data[4] * other.data[13] + data[8] * other.data[14] +
                 data[12] * other.data[15];
    m.data[13] = data[1] * other.data[12] + data[5] * other.data[13] + data[9] * other.data[14] +
                 data[13] * other.data[15];
    m.data[14] = data[2] * other.data[12] + data[6] * other.data[13] + data[10] * other.data[14] +
                 data[14] * other.data[15];
    m.data[15] = data[3] * other.data[12] + data[7] * other.data[13] + data[11] * other.data[14] +
                 data[15] * other.data[15];

    *this = m;
    return *this;
}

Vector Matrix::operator*(const Vector& other) const {
    return Vector(data[0] * other.x + data[4] * other.y + data[12] * 1.0f,
                  data[1] * other.x + data[5] * other.y + data[13] * 1.0f) /
           (data[3] * other.x + data[7] * other.y + data[15] * 1.0f);
}

Matrix Matrix::operator*(const Matrix& other) const {
    Matrix m = *this;
    m *= other;
    return m;
}

Matrix& Matrix::unit() {
    data[0] = 1.0f;
    data[1] = 0.0f;
    data[2] = 0.0f;
    data[3] = 0.0f;

    data[4] = 0.0f;
    data[5] = 1.0f;
    data[6] = 0.0f;
    data[7] = 0.0f;

    data[8] = 0.0f;
    data[9] = 0.0f;
    data[10] = 1.0f;
    data[11] = 0.0f;

    data[12] = 0.0f;
    data[13] = 0.0f;
    data[14] = 0.0f;
    data[15] = 1.0f;
    return *this;
}

Matrix& Matrix::scale(const Vector& v) {
    data[0] *= v.x;
    data[1] *= v.x;
    data[2] *= v.x;
    data[3] *= v.x;

    data[4] *= v.y;
    data[5] *= v.y;
    data[6] *= v.y;
    data[7] *= v.y;
    return *this;
}

Matrix& Matrix::transform(const Vector& v) {
    data[12] += data[0] * v.x + data[4] * v.y;
    data[13] += data[1] * v.x + data[5] * v.y;
    data[14] += data[2] * v.x + data[6] * v.y;
    data[15] += data[3] * v.x + data[7] * v.y;
    return *this;
}

const float* Matrix::getData() const {
    return data;
}