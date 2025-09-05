#pragma once
#include "stdafx.h"
#include "Vector.h"

struct FVector4
{
    float X;
    float Y;
    float Z;
    float W;
    FVector4(float _x = 0, float _y = 0, float _z = 0, float _w = 1) : X(_x), Y(_y), Z(_z), W(_w) {}

    // 동차좌표를 3D로 변환 (W로 나눔)
    void Homogenize()
    {
        // 예외 처리 해야됨
        //if (W == 0.f)
           // throw std::runtime_error("Cannot homogenize with W = 0");

        X /= W;
        Y /= W;
        Z /= W;
        W = 1.f;
    }

    // 내적 (4차원)
    float Dot(const FVector4& Other) const
    {
        return X * Other.X + Y * Other.Y + Z * Other.Z + W * Other.W;
    }

    // 길이 (4차원)
    float Length() const
    {
        return sqrtf(X * X + Y * Y + Z * Z + W * W);
    }

    // 길이 (XYZ만)
    float Length3() const
    {
        return sqrtf(X * X + Y * Y + Z * Z);
    }

    // 정규화 (4차원)
    FVector4 Normalized() const
    {
        float len = Length();
        if (len == 0.f) return FVector4();
        return FVector4(X / len, Y / len, Z / len, W / len);
    }

    // XYZ만 정규화
    FVector4 Normalized3() const
    {
        float len = Length3();
        if (len == 0.f) return FVector4();
        return FVector4(X / len, Y / len, Z / len, W);
    }

    // 연산자 오버로딩
    FVector4 operator+(const FVector4& Other) const
    {
        return FVector4(X + Other.X, Y + Other.Y, Z + Other.Z, W + Other.W);
    }

    FVector4 operator-(const FVector4& Other) const
    {
        return FVector4(X - Other.X, Y - Other.Y, Z - Other.Z, W - Other.W);
    }

    FVector4 operator*(float Scalar) const
    {
        return FVector4(X * Scalar, Y * Scalar, Z * Scalar, W * Scalar);
    }

    FVector4 operator/(float Scalar) const
    {
        return FVector4(X / Scalar, Y / Scalar, Z / Scalar, W / Scalar);
    }
};