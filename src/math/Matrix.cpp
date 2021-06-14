#include "Matrix.h"

void MatrixUtils::setTransform(float width, float height, Matrix& matrix) {
    matrix[0] = 2.0f / width;
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;

    matrix[4] = 0.0f;
    matrix[5] = -2.0f / height;
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;

    matrix[8] = 0.0f;
    matrix[9] = 0.0f;
    matrix[10] = 1.0f;
    matrix[11] = 0.0f;

    matrix[12] = -1.0f;
    matrix[13] = 1.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;
}