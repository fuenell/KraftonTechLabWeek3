#pragma once
#include "stdafx.h"
#include "Vector.h"
#include "Matrix.h"   // FMatrix 선언부가 들어있는 헤더
#include <math.h>

struct FMatrix;

struct FQuaternion
{
    float X, Y, Z, W;

    FQuaternion(float x = 0, float y = 0, float z = 0, float w = 1) : X(x), Y(y), Z(z), W(w) {}

    static FQuaternion Identity() { return FQuaternion(0, 0, 0, 1); }

    void Normalize() {
        float s = X * X + Y * Y + Z * Z + W * W;
        if (s > 0.0f) {
            float inv = 1.0f / sqrtf(s);
            X *= inv; Y *= inv; Z *= inv; W *= inv;
        }
    }
    FQuaternion GetNormalized() const { FQuaternion q(*this); q.Normalize(); return q; }

    // 곱셈(회전 합성): (this 먼저, 그 다음 b)
    FQuaternion operator*(const FQuaternion& b) const {
        return FQuaternion(
            W * b.X + X * b.W + Y * b.Z - Z * b.Y,
            W * b.Y - X * b.Z + Y * b.W + Z * b.X,
            W * b.Z + X * b.Y - Y * b.X + Z * b.W,
            W * b.W - X * b.X - Y * b.Y - Z * b.Z
        );
    }

    // 켤레/역수(단위 쿼터니언 가정 시 conj가 inverse)
    FQuaternion Conjugate() const { return FQuaternion(-X, -Y, -Z, W); }
    FQuaternion Inverse() const {
        float s = X * X + Y * Y + Z * Z + W * W;
        if (s <= 0.0f) return FQuaternion();
        float inv = 1.0f / s;
        return FQuaternion(-X * inv, -Y * inv, -Z * inv, W * inv);
    }

    // 벡터 회전 (RHS, row-vector): v' = q * v * q^{-1}
    FVector Rotate(const FVector& v) const {
        FQuaternion qv(v.X, v.Y, v.Z, 0.0f);
        FQuaternion r = (*this) * qv * this->Inverse();
        return FVector(r.X, r.Y, r.Z);
    }

    // 동차좌표 벡터 회전: XYZ만 회전, W는 보존
    FVector4 Rotate(const FVector4& v) const {
        FVector rotated = Rotate(FVector(v.X, v.Y, v.Z));
        return FVector4(rotated.X, rotated.Y, rotated.Z, v.W);
    }

    // 축-각도(RHS) axis는 단위벡터 가정
    static FQuaternion FromAxisAngle(const FVector& axis, float angleRad) {
        float h = angleRad * 0.5f; float s = sinf(h);
        return FQuaternion(axis.X * s, axis.Y * s, axis.Z * s, cosf(h));
    }

    // 오일러(RHS, row-vector, Z-up) — 적용 순서: Yaw(Z) → Pitch(X) → Roll(Y)
    // 주의: 행벡터가 오른쪽에서 곱해지므로 "적용 순서"는 실제 내부 곱셈의 반대순으로 조립될 수 있음.
    static FQuaternion FromEuler(float pitchX, float yawZ, float rollY) {
        float cx = cosf(0.5f * pitchX), sx = sinf(0.5f * pitchX);
        float cy = cosf(0.5f * yawZ), sy = sinf(0.5f * yawZ);
        float cz = cosf(0.5f * rollY), sz = sinf(0.5f * rollY);

        // Z(Yaw) * X(Pitch) * Y(Roll) 순
        FQuaternion qx(sx, 0, 0, cx);
        FQuaternion qy(0, sz, 0, cz);
        FQuaternion qz(0, 0, sy, cy);
        FQuaternion q = qz * qx * qy; // 주어진 정의에 맞춰 조립
        q.Normalize();
        return q;
    }

    // LookRotation (RHS, row-vector)
    // 표준 RHS 기준: right = normalize(up × forward), up' = normalize(forward × right)
    static FQuaternion LookRotation(const FVector& forward, const FVector& up)
    {
        FVector f = forward.GetNormalized();
        FVector r = f.Cross(up).GetNormalized();
        FVector u = r.Cross(f); // Up' = R × F

        // 행벡터(row-vector) 기준으로 "행"을 채운다.
        float m00 = r.X, m01 = r.Y, m02 = r.Z;
        float m10 = u.X, m11 = u.Y, m12 = u.Z;
        float m20 = f.X, m21 = f.Y, m22 = f.Z;

        float tr = m00 + m11 + m22;
        FQuaternion q;
        if (tr > 0.0f) {
            float s = sqrtf(tr + 1.0f) * 2.0f;
            q.W = 0.25f * s;
            q.X = (m21 - m12) / s;
            q.Y = (m02 - m20) / s;
            q.Z = (m10 - m01) / s;
        }
        else if (m00 > m11 && m00 > m22) {
            float s = sqrtf(1.0f + m00 - m11 - m22) * 2.0f;
            q.W = (m21 - m12) / s;
            q.X = 0.25f * s;
            q.Y = (m01 + m10) / s;
            q.Z = (m02 + m20) / s;
        }
        else if (m11 > m22) {
            float s = sqrtf(1.0f + m11 - m00 - m22) * 2.0f;
            q.W = (m02 - m20) / s;
            q.X = (m01 + m10) / s;
            q.Y = 0.25f * s;
            q.Z = (m12 + m21) / s;
        }
        else {
            float s = sqrtf(1.0f + m22 - m00 - m11) * 2.0f;
            q.W = (m10 - m01) / s;
            q.X = (m02 + m20) / s;
            q.Y = (m12 + m21) / s;
            q.Z = 0.25f * s;
        }
        q.Normalize();
        return q;
    }

    // 행렬 변환 (RHS, row-vector) — v' = v * R
    FMatrix ToMatrix() const;
};