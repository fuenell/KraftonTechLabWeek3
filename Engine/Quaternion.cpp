#include "stdafx.h"
#include "Quaternion.h"
#include "Matrix.h"

FMatrix FQuaternion::ToMatrix() const {
    FMatrix M = FMatrix::IdentityMatrix();
    float xx = X * X, yy = Y * Y, zz = Z * Z;
    float xy = X * Y, xz = X * Z, yz = Y * Z;
    float wx = W * X, wy = W * Y, wz = W * Z;

    // RH + row-vector 회전블록
    M.M[0][0] = 1.0f - 2.0f * (yy + zz);
    M.M[0][1] = 2.0f * (xy - wz);
    M.M[0][2] = 2.0f * (xz + wy);

    M.M[1][0] = 2.0f * (xy + wz);
    M.M[1][1] = 1.0f - 2.0f * (xx + zz);
    M.M[1][2] = 2.0f * (yz - wx);

    M.M[2][0] = 2.0f * (xz - wy);
    M.M[2][1] = 2.0f * (yz + wx);
    M.M[2][2] = 1.0f - 2.0f * (xx + yy);

    M.M[3][3] = 1.0f;
    return M;
}