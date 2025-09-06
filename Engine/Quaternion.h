#pragma once
#include "stdafx.h"
#include "Vector.h"
#include "Vector4.h"
#include "Matrix.h"
#include <math.h>

// ===================== 유틸 =====================
#ifndef PI
#define PI 3.14159265358979323846
#endif
inline float ToRad(float d) { return d * (float)(PI / 180.0); }

struct FQuaternion
{
    // (x,y,z,w) with w = scalar
    // 즉, 쿼터니언은 W가 실수부이고 x, y, z는 허수부이다.
    float X, Y, Z, W;

    FQuaternion(float x = 0, float y = 0, float z = 0, float w = 1) : X(x), Y(y), Z(z), W(w) {}

    static FQuaternion Identity() { return FQuaternion(0, 0, 0, 1); }

    // 기본 연산
    float Dot(const FQuaternion& o) const { return X * o.X + Y * o.Y + Z * o.Z + W * o.W; }

    float Length() const { return sqrtf(X * X + Y * Y + Z * Z + W * W); }

    FQuaternion Normalized() const {
        float l = Length();
        if (l <= 0.0f) return Identity();
        float inv = 1.0f / l;
        return FQuaternion(X * inv, Y * inv, Z * inv, W * inv);
    }
    // 켤레
    FQuaternion Conjugate() const { return FQuaternion(-X, -Y, -Z, W); }

    FQuaternion Inverse() const {
        // 단위쿼터니언 가정(회전): inverse = conjugate
        // 일반형: conj / |q|^2
        float n2 = X * X + Y * Y + Z * Z + W * W;
        if (n2 <= 0.0f) return Identity();
        float inv = 1.0f / n2;
        return FQuaternion(-X * inv, -Y * inv, -Z * inv, W * inv);
    }

    // ====== 행벡터 친화 합성 ======
    // 표준 해밀턴 곱(H): "먼저 a, 그 다음 b"를 column 규약으로 표현하면 b ⊗ a.
    // 우리는 row 규약이므로 "행렬 곱 순서와 동일"하도록 정의:
    //   ToMatrixRow(a * b) == ToMatrixRow(a) * ToMatrixRow(b)
    //   → operator*는 H(b,a)로 구현한다.
    static FQuaternion Hamilton(const FQuaternion& p, const FQuaternion& q) {
        // 표준 해밀턴 곱: q ⊗ p (column 규약: 먼저 p, 그 다음 q)
        return FQuaternion(
            q.W * p.X + q.X * p.W + q.Y * p.Z - q.Z * p.Y,
            q.W * p.Y - q.X * p.Z + q.Y * p.W + q.Z * p.X,
            q.W * p.Z + q.X * p.Y - q.Y * p.X + q.Z * p.W,
            q.W * p.W - q.X * p.X - q.Y * p.Y - q.Z * p.Z
        );
    }

    FQuaternion operator*(const FQuaternion& rhs) const {
        // row 규약 합성: 먼저 this, 그 다음 rhs
        // → 표준 해밀턴으로는 Hamilton(this, rhs)가 아니라 Hamilton(rhs, this)
        return Hamilton(rhs, *this);
    }

    // 스칼라 보간/SLERP
    // 계산 빠름 (성분끼리 그냥 선형 보간)
    // 근사적인 회전 → 두 회전 사이가 "대략적"으로 부드럽게 이어짐
    // 짧은 각도 차이에서는 SLERP와 거의 차이가 없음
    static FQuaternion Lerp(const FQuaternion& a, const FQuaternion& b, float t) {
        FQuaternion r(
            a.X + (b.X - a.X) * t,
            a.Y + (b.Y - a.Y) * t,
            a.Z + (b.Z - a.Z) * t,
            a.W + (b.W - a.W) * t
        );
        return r.Normalized();
    }
    // 두 쿼터니온이 만드는 4차원 단위구면(S³) 위의 "최단 호"를 따라 회전
    // 회전 속도가 일정해 보임(등속 회전)
    // 정확하지만 계산이 조금 더 무거움(삼각함수 사용)
    // 쿼터니온은 단위 4차원 벡터 (S³ 구면 위의 점)
    // 두 쿼터니온 a, b는 같은 구면 위의 점
    // 따라서 "구 위의 최단 경로(호)"를 따라가며 보간하는 게 SLERP
    // 즉, a에서 b로 가는 대원(geodesic) 을 따라t만큼 이동하는 원리
    static FQuaternion Slerp(FQuaternion a, FQuaternion b, float t) {
        float cosv = a.Dot(b);
        if (cosv < 0.0f) { b = FQuaternion(-b.X, -b.Y, -b.Z, -b.W); cosv = -cosv; }
        const float EPS = 1e-6f;
        if (cosv > 1.0f - EPS) return Lerp(a, b, t);
        float ang = acosf(cosv);
        float sA = sinf((1.0f - t) * ang);
        float sB = sinf(t * ang);
        float invS = 1.0f / sinf(ang);
        FQuaternion r(
            (a.X * sA + b.X * sB) * invS,
            (a.Y * sA + b.Y * sB) * invS,
            (a.Z * sA + b.Z * sB) * invS,
            (a.W * sA + b.W * sB) * invS
        );
        return r.Normalized();
    }

    // 생성기
    static FQuaternion FromAxisAngle(FVector axis, float rad) {
        float len = sqrtf(axis.X * axis.X + axis.Y * axis.Y + axis.Z * axis.Z);
        if (len <= 0.0f) return Identity();
        float inv = 1.0f / len; axis.X *= inv; axis.Y *= inv; axis.Z *= inv;
        float s = sinf(rad * 0.5f), c = cosf(rad * 0.5f);
        return FQuaternion(axis.X * s, axis.Y * s, axis.Z * s, c);
    }

    // 행벡터 규약: X→Y→Z 순서라면 실제 행렬은 S*Rx*Ry*Rz 같은 식으로 오른쪽에 쌓임.
    // 여기서는 "오일러 X, Y, Z를 그 순서로 적용"하도록 구성(카메라 pitch-yaw 등 용도에 맞게 바꿔 써).
    static FQuaternion FromEulerXYZ(float rx, float ry, float rz) {
        FQuaternion qx = FromAxisAngle(FVector(1, 0, 0), rx);
        FQuaternion qy = FromAxisAngle(FVector(0, 1, 0), ry);
        FQuaternion qz = FromAxisAngle(FVector(0, 0, 1), rz);
        // 먼저 X, 그 다음 Y, 그 다음 Z → row 규약 합성
        return qx * qy * qz;
    }

    // forward/up으로부터 LookRotation
    // forward, up은 정규화 안 되어 있어도 됨.
// quaternion.h
    static FQuaternion LookRotation(const FVector& forward, const FVector& up)
    {
        FVector f = forward; f.Normalize();          // Forward(+Y)
        FVector upN = up;     upN.Normalize();       // World Up(+Z)

        // f‖upN 방지
        if (fabsf(f.Dot(upN)) > 0.999f) {
            upN = (fabsf(f.Z) < 0.9f) ? FVector(0, 0, 1) : FVector(1, 0, 0);
        }

        // RH, Z-up: Right = f × up , Up = Right × f
        FVector r = f.Cross(upN);  r.Normalize();
        FVector u = r.Cross(f);    // 자동 직교

        // ⬇ 열 순서 [r, f, u]
        float m00 = r.X, m01 = f.X, m02 = u.X;
        float m10 = r.Y, m11 = f.Y, m12 = u.Y;
        float m20 = r.Z, m21 = f.Z, m22 = u.Z;

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
        return q.Normalized();
    }


    // 두 벡터 a→b 회전
    static FQuaternion FromTo(FVector a, FVector b) {
        a.Normalize(); b.Normalize();
        float c = a.Dot(b);
        if (c > 0.9999f) return Identity();
        if (c < -0.9999f) {
            // 반대 방향: a와 직교하는 축으로 180도
            FVector ortho = (fabsf(a.X) < 0.9f) ? FVector(1, 0, 0) : FVector(0, 1, 0);
            FVector axis = a.Cross(ortho).GetNormalized();
            return FromAxisAngle(axis, (float)PI);
        }
        FVector axis = a.Cross(b);
        float s = sqrtf((1.0f + c) * 2.0f);
        float invs = 1.0f / s;
        return FQuaternion(axis.X * invs, axis.Y * invs, axis.Z * invs, s * 0.5f).Normalized();
    }

    // 벡터 회전 (쿼터니언 샌드위치)
    FVector Rotate(const FVector& v) const {
        FQuaternion qv(v.X, v.Y, v.Z, 0.0f);
        FQuaternion inv = Inverse();
        //// tmp = q ⊗ v
        //FQuaternion tmp = Hamilton(qv, *this);   // Hamilton(p,q)=q⊗p 이므로 (qv, q)

        //// r = (q ⊗ v) ⊗ q^{-1}
        //FQuaternion r = Hamilton(inv, tmp);      // (tmp, inv) => tmp ⊗ inv

        // inv ⊗ v
        FQuaternion tmp = Hamilton(qv, inv);   // Hamilton(p,q) = q ⊗ p

        // (inv ⊗ v) ⊗ q
        FQuaternion r = Hamilton(*this, tmp);  // => tmp ⊗ q
        return FVector(r.X, r.Y, r.Z);
    }

    // === 행벡터 규약 회전행렬 생성 ===
    // v' = v * R
    // quaternion.h
    FMatrix ToMatrixRow() const {
        FQuaternion q = Normalized();
        float x = q.X, y = q.Y, z = q.Z, w = q.W;

        float xx = x * x, yy = y * y, zz = z * z;
        float xy = x * y, xz = x * z, yz = y * z;
        float wx = w * x, wy = w * y, wz = w * z;

        FMatrix R = FMatrix::IdentityMatrix();
        // row0 = Right (X)
        R.M[0][0] = 1.0f - 2.0f * (yy + zz);
        R.M[0][1] = 2.0f * (xy + wz);
        R.M[0][2] = 2.0f * (xz - wy);
        R.M[0][3] = 0.0f;

        // row1 = Up (Z)  ← Z-up이므로 표준 행렬의 row2를 여기로
        R.M[1][0] = 2.0f * (xz + wy);
        R.M[1][1] = 2.0f * (yz - wx);
        R.M[1][2] = 1.0f - 2.0f * (xx + yy);
        R.M[1][3] = 0.0f;

        // row2 = Forward (Y)  ← 표준 행렬의 row1을 여기로
        R.M[2][0] = 2.0f * (xy - wz);
        R.M[2][1] = 1.0f - 2.0f * (xx + zz);
        R.M[2][2] = 2.0f * (yz + wx);
        R.M[2][3] = 0.0f;

        return R;
    }
    FMatrix ToMatrixRowForModel() const {
        FQuaternion q = Normalized();
        float x = q.X, y = q.Y, z = q.Z, w = q.W;

        float xx = x * x, yy = y * y, zz = z * z;
        float xy = x * y, xz = x * z, yz = y * z;
        float wx = w * x, wy = w * y, wz = w * z;

        FMatrix R = FMatrix::IdentityMatrix();

        // row0 = Right (X)
        R.M[0][0] = 1.0f - 2.0f * (yy + zz);
        R.M[0][1] = 2.0f * (xy + wz);
        R.M[0][2] = 2.0f * (xz - wy);
        R.M[0][3] = 0.0f;

        // row1 = Forward (Y)
        R.M[1][0] = 2.0f * (xy - wz);
        R.M[1][1] = 1.0f - 2.0f * (xx + zz);
        R.M[1][2] = 2.0f * (yz + wx);
        R.M[1][3] = 0.0f;

        // row2 = Up (Z)
        R.M[2][0] = 2.0f * (xz + wy);
        R.M[2][1] = 2.0f * (yz - wx);
        R.M[2][2] = 1.0f - 2.0f * (xx + yy);
        R.M[2][3] = 0.0f;

        // row3
        R.M[3][0] = R.M[3][1] = R.M[3][2] = 0.0f;
        R.M[3][3] = 1.0f;
        return R;
    }

    // 회전행렬(행벡터 규약, 상단3x3) → 쿼터니언
    static FQuaternion FromMatrixRow(const FMatrix& R) {
        float m00 = R.M[0][0], m01 = R.M[0][1], m02 = R.M[0][2];
        float m10 = R.M[1][0], m11 = R.M[1][1], m12 = R.M[1][2];
        float m20 = R.M[2][0], m21 = R.M[2][1], m22 = R.M[2][2];

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
        return q.Normalized();
    }

};

// ===== 편의 함수: 카메라용 뷰/모델 행렬 =====

// 카메라 뷰 (row-vector): V = T(-eye) * R^T
inline FMatrix MakeViewRow(const FVector& eye, const FQuaternion& q)
{
    //FMatrix R = q.ToMatrixRow();
    //FMatrix Rt = FMatrix::Transpose(R);               // 회전의 역변환
    //FMatrix Tinv = FMatrix::TranslationRow(-eye.X, -eye.Y, -eye.Z);
    //return Tinv * Rt;
    // 로컬 기준축(+X Right, +Y Forward, +Z Up)을 회전해서 월드축으로
    FVector r = q.Rotate({1,0,0}).GetNormalized(); // Right
    FVector f = q.Rotate({0,1,0}).GetNormalized(); // Forward
    FVector u = q.Rotate({0,0,1}).GetNormalized(); // Up

    FVector fb = FVector(-f.X, -f.Y, -f.Z);        // ★ 예전 코드의 f(뒤벡터)와 동일: fb = -Forward

    FMatrix V = FMatrix::IdentityMatrix();

    // === 회전부 (예전 LookAt과 정확히 동일한 배치) ===
    // row0 = s(=Right), row1 = u(=Up), row2 = f(=backward = -Forward)
    V.M[0][0] = r.X;  V.M[0][1] = r.Y;  V.M[0][2] = r.Z;  V.M[0][3] = 0.0f;
    V.M[1][0] = u.X;  V.M[1][1] = u.Y;  V.M[1][2] = u.Z;  V.M[1][3] = 0.0f;
    V.M[2][0] = fb.X; V.M[2][1] = fb.Y; V.M[2][2] = fb.Z; V.M[2][3] = 0.0f;

    // === 번역부 (예전 LookAt과 동일 수식) ===
    V.M[3][0] = -(eye.X * r.X + eye.Y * r.Y + eye.Z * r.Z);
    V.M[3][1] = -(eye.X * u.X + eye.Y * u.Y + eye.Z * u.Z);
    V.M[3][2] = -(eye.X * fb.X + eye.Y * fb.Y + eye.Z * fb.Z);
    V.M[3][3] = 1.0f;

    return V;
}

// 모델행렬 (row-vector): M = S * R * T
inline FMatrix MakeModelRow(const FVector& pos, const FQuaternion& rot, const FVector& scl)
{
    FMatrix S = FMatrix::Scale(scl.X, scl.Y, scl.Z);
    FMatrix R = rot.ToMatrixRowForModel();
    FMatrix T = FMatrix::TranslationRow(pos.X, pos.Y, pos.Z);
    return S * R * T;
}

