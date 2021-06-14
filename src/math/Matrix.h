#ifndef MATRIX_H
#define MATRIX_H

typedef float Matrix[16];

namespace MatrixUtils {
    void setTransform(float width, float height, Matrix& matrix);
}

#endif