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
inline float ToDeg(float r) { return r * (180.0f / (float)PI); }
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
    static FQuaternion FromEulerXYZ(float rx, float ry, float rz) {
        FQuaternion qx = FromAxisAngle(FVector(1, 0, 0), rx);
        FQuaternion qy = FromAxisAngle(FVector(0, 1, 0), ry);
        FQuaternion qz = FromAxisAngle(FVector(0, 0, 1), rz);
        // 먼저 X, 그 다음 Y, 그 다음 Z → row 규약 합성
        return qx * qy * qz;
    }
    static FQuaternion FromEulerXYZDeg(float degX, float degY, float degZ) {
        return FromEulerXYZ(ToRad(degX), ToRad(degY), ToRad(degZ));
    }
    // TODO : 오브젝트에 쓰려면 조정이 필요함. 카메라 전용이었어서 잘 작동하지 않을 수 있음
    static FQuaternion LookRotation(const FVector& fwd, const FVector& up)
    {
        FVector F = fwd; F.Normalize();

        // 1) up을 F에 직교하도록 정규화(Gram-Schmidt)
        FVector Uref = up;
        if (Uref.Length() < 1e-12f) Uref = FVector(0, 0, 1);
        // U = up - proj_up_on_F
        FVector U = (Uref - F * F.Dot(Uref));
        float uLen2 = U.Length();
        if (uLen2 < 1e-12f) {
            // fwd ≈ up 인 경우 대체 업벡터 선택
            U = (fabsf(F.Z) < 0.9f) ? FVector(0, 0, 1) : FVector(1, 0, 0);
            U = (U - F * F.Dot(U));
        }
        U.Normalize();

        // 2) RH 유지: R = U × F, U = F × R
        FVector R = F.Cross(U).GetNormalized();
        U = R.Cross(F).GetNormalized(); // 재직교(수치안정)

        // 3) 열에 축을 넣는다 (columns are axes)
        FMatrix M = FMatrix::IdentityMatrix();
        // col0 = Right = R
        M.M[0][0] = R.X; M.M[0][1] = F.X; M.M[0][2] = U.X;
        // col1 = Forward = +F   ★ 오브젝트는 +Y가 전방
        M.M[1][0] = R.Y; M.M[1][1] = F.Y; M.M[2][1] = U.Y;
        // col2 = Up = U
        M.M[2][0] = R.Z; M.M[1][2] = F.Z; M.M[2][2] = U.Z;

        return FQuaternion::FromMatrixRow(M).Normalized();
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
        // tmp = q ⊗ v
        FQuaternion tmp = Hamilton(qv, *this);   // Hamilton(p,q)=q⊗p 이므로 (qv, q)
        // r = (q ⊗ v) ⊗ q^{-1}
        FQuaternion r = Hamilton(inv, tmp);      // (tmp, inv) => tmp ⊗ inv

        return FVector(r.X, r.Y, r.Z);
    }
    // === COLUMNS are axes (row-vector) ===
    // col0 = q·(+X) = Right
    FMatrix ToMatrixRow() const {
        FQuaternion q = Normalized();

        float x = q.X, y = q.Y, z = q.Z, w = q.W;
        float xx = x * x, yy = y * y, zz = z * z;
        float xy = x * y, xz = x * z, yz = y * z;
        float wx = w * x, wy = w * y, wz = w * z;

        FMatrix R = FMatrix::IdentityMatrix();
        // === columns are axes (col0=+X, col1=+Y, col2=+Z) ===
        R.M[0][0] = 1.0f - 2.0f * (yy + zz);
        R.M[0][1] = 2.0f * (xy - wz);
        R.M[0][2] = 2.0f * (xz + wy);

        R.M[1][0] = 2.0f * (xy + wz);
        R.M[1][1] = 1.0f - 2.0f * (xx + zz);
        R.M[1][2] = 2.0f * (yz - wx);

        R.M[2][0] = 2.0f * (xz - wy);
        R.M[2][1] = 2.0f * (yz + wx);
        R.M[2][2] = 1.0f - 2.0f * (xx + yy);
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
    // 규약: row-vector, columns are axes, R = Rx * Ry * Rz  (X→Y→Z 순서)
    FVector ToEulerXYZ() const
    {
        FMatrix R = ToMatrixRow();
        // 유도 결과: R[0][2] = -sin(ry)
        float sy = -R.M[0][2];
        if (sy > 1.0f) sy = 1.0f;
        if (sy < -1.0f) sy = -1.0f;

        float ry = asinf(sy);
        float cy = cosf(ry);

        float rx, rz;
        const float EPS = 1e-6f;

        if (fabsf(cy) > EPS) {
            // 일반 케이스:
            // R[1][2] =  sin(rx)*cos(ry)
            // R[2][2] =  cos(rx)*cos(ry)
            rx = atan2f(R.M[1][2], R.M[2][2]);

            // R[0][1] =  sin(rz)*cos(ry)
            // R[0][0] =  cos(rz)*cos(ry)
            rz = atan2f(R.M[0][1], R.M[0][0]);
        }
        else {
            // 짐벌락(cos(ry) ~ 0): rz 미정 → rz=0으로 두고 rx만 안정적으로 계산
            rz = 0.0f;

            // 부호 주의: ry≈+π/2, -π/2 모두에서 동작하도록 sign(sy) 보정
            // ry=+π/2: R[1][0]=sin(rx), R[1][1]=cos(rx)
            // ry=-π/2: R[1][0]=-sin(rx), R[1][1]=cos(rx)
            float sgn = (sy >= 0.0f) ? 1.0f : -1.0f;
            rx = atan2f(sgn * R.M[1][0], R.M[1][1]);
        }

        return FVector(rx, ry, rz); // (X, Y, Z) 라디안
    }
    FVector ToEulerXYZ_Deg() const {
        FVector r = ToEulerXYZ();
        const float R2D = 180.0f / (float)PI;
        return FVector(r.X * R2D, r.Y * R2D, r.Z * R2D);
    }
    // ============== Quaternion -> EulerXYZ (Rx * Ry * Rz) ==============
// 반환: FVector(rx, ry, rz) 라디안
    static FVector EulerXYZFrom(const FQuaternion& q)
    {
        FMatrix R = q.ToMatrixRow();

        // 행벡터 + columns=axes + R = Rx*Ry*Rz 전개 결과:
        // R[0][2] = -sin(ry)
        float sy = -R.M[0][2];
        if (sy > 1.0f) sy = 1.0f;
        if (sy < -1.0f) sy = -1.0f;

        float ry = asinf(sy);
        float cy = cosf(ry);

        float rx, rz;
        const float EPS = 1e-6f;

        if (fabsf(cy) > EPS) {
            // R[1][2] =  sin(rx)*cos(ry),  R[2][2] = cos(rx)*cos(ry)
            rx = atan2f(R.M[1][2], R.M[2][2]);

            // R[0][1] =  sin(rz)*cos(ry),  R[0][0] = cos(rz)*cos(ry)
            rz = atan2f(R.M[0][1], R.M[0][0]);
        }
        else {
            // 짐벌락: rz 미정 → rz=0, rx만 안정적으로 뽑기
            rz = 0.0f;

            // ry ≈ ±π/2 에서 부호 일관성 확보용 보정
            float sgn = (sy >= 0.0f) ? 1.0f : -1.0f;
            // ry=+π/2: R[1][0]= sin(rx), R[1][1]=cos(rx)
            // ry=-π/2: R[1][0]=-sin(rx), R[1][1]=cos(rx)
            rx = atan2f(sgn * R.M[1][0], R.M[1][1]);
        }

        return FVector(rx, ry, rz);
    }

    // 반환: FVector(rx, ry, rz) 도(deg)
    static FVector EulerXYZDegFrom(const FQuaternion& q)
    {
        FVector r = EulerXYZFrom(q);
        return FVector(ToDeg(r.X), ToDeg(r.Y), ToDeg(r.Z));
    }
    FQuaternion RotatedWorldAxisAngle(const FVector& worldAxis, float radians) const
    {
        FQuaternion qW = FromAxisAngle(worldAxis, radians);
        return (qW * *this).Normalized();
    }
    // In-place 버전
    void RotateWorldAxisAngleInPlace(const FVector& worldAxis, float radians)
    {
        *this = RotatedWorldAxisAngle(worldAxis, radians);
    }

    // 월드 오일러(X→Y→Z)로 누적: q' = (Rx * Ry * Rz) * q
    FQuaternion RotatedWorldEulerXYZ(float rx, float ry, float rz) const
    {
        FQuaternion qx = FromAxisAngle(FVector(1, 0, 0), rx);
        FQuaternion qy = FromAxisAngle(FVector(0, 1, 0), ry);
        FQuaternion qz = FromAxisAngle(FVector(0, 0, 1), rz);
        FQuaternion qW = qx * qy * qz;
        return (qW * *this).Normalized();
    }
    void RotateWorldEulerXYZInPlace(float rx, float ry, float rz)
    {
        *this = RotatedWorldEulerXYZ(rx, ry, rz);
    }

    // 월드 Yaw/Pitch/Roll 누적 (Z=Yaw, X=Pitch, Y=Roll; Z-up, Forward=+Y 규약)
    // 합성: Rz(yaw) * Rx(pitch) * Ry(roll)
    FQuaternion RotatedWorldYawPitchRoll(float yawZ, float pitchX, float rollY) const
    {
        FQuaternion qz = FromAxisAngle(FVector(0, 0, 1), yawZ);
        FQuaternion qx = FromAxisAngle(FVector(1, 0, 0), pitchX);
        FQuaternion qy = FromAxisAngle(FVector(0, 1, 0), rollY);
        return (qz * qx * qy * *this).Normalized();
    }
    void RotateWorldYawPitchRollInPlace(float yawZ, float pitchX, float rollY)
    {
        *this = RotatedWorldYawPitchRoll(yawZ, pitchX, rollY);
    }

    // ============================================================
    // 월드 축 기준 누적 회전 (도 단위) — GUI용
    // ============================================================
    FQuaternion RotatedWorldAxisAngleDeg(const FVector& worldAxis, float degrees) const
    {
        return RotatedWorldAxisAngle(worldAxis, ToRad(degrees));
    }
    void RotateWorldAxisAngleDegInPlace(const FVector& worldAxis, float degrees)
    {
        *this = RotatedWorldAxisAngleDeg(worldAxis, degrees);
    }

    FQuaternion RotatedWorldEulerXYZDeg(float degX, float degY, float degZ) const
    {
        return RotatedWorldEulerXYZ(ToRad(degX), ToRad(degY), ToRad(degZ));
    }
    void RotateWorldEulerXYZDegInPlace(float degX, float degY, float degZ)
    {
        *this = RotatedWorldEulerXYZDeg(degX, degY, degZ);
    }

    FQuaternion RotatedWorldYawPitchRollDeg(float yawZ_deg, float pitchX_deg, float rollY_deg) const
    {
        return RotatedWorldYawPitchRoll(ToRad(yawZ_deg), ToRad(pitchX_deg), ToRad(rollY_deg));
    }
    void RotateWorldYawPitchRollDegInPlace(float yawZ_deg, float pitchX_deg, float rollY_deg)
    {
        *this = RotatedWorldYawPitchRollDeg(yawZ_deg, pitchX_deg, rollY_deg);
    }

    // ============================================================
    // 편의: 월드 축 단일 축(라디안) 회전
    // ============================================================
    FQuaternion RotatedWorldX(float radians) const { return RotatedWorldAxisAngle(FVector(1, 0, 0), radians); }
    FQuaternion RotatedWorldY(float radians) const { return RotatedWorldAxisAngle(FVector(0, 1, 0), radians); }
    FQuaternion RotatedWorldZ(float radians) const { return RotatedWorldAxisAngle(FVector(0, 0, 1), radians); }
    void RotateWorldXInPlace(float radians) { *this = RotatedWorldX(radians); }
    void RotateWorldYInPlace(float radians) { *this = RotatedWorldY(radians); }
    void RotateWorldZInPlace(float radians) { *this = RotatedWorldZ(radians); }

    // 도 단위 단일 축
    FQuaternion RotatedWorldXDeg(float deg) const { return RotatedWorldX(ToRad(deg)); }
    FQuaternion RotatedWorldYDeg(float deg) const { return RotatedWorldY(ToRad(deg)); }
    FQuaternion RotatedWorldZDeg(float deg) const { return RotatedWorldZ(ToRad(deg)); }
    void RotateWorldXDegInPlace(float deg) { *this = RotatedWorldXDeg(deg); }
    void RotateWorldYDegInPlace(float deg) { *this = RotatedWorldYDeg(deg); }
    void RotateWorldZDegInPlace(float deg) { *this = RotatedWorldZDeg(deg); }
};

// 모델행렬 (row-vector): M = S * R * T
inline FMatrix MakeModelRow(const FVector& pos, const FQuaternion& rot, const FVector& scl)
{
    FMatrix S = FMatrix::Scale(scl.X, scl.Y, scl.Z);
    FMatrix R = rot.ToMatrixRow();
    FMatrix T = FMatrix::TranslationRow(pos.X, pos.Y, pos.Z);
    return S * R * T;
}

