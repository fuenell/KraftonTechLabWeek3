#pragma once
#include "stdafx.h"
#include "Vector.h"
#include "Matrix.h"   // 너가 올린 FMatrix 선언부가 들어있는 헤더 이름에 맞춰 변경

// =====================
// UCamera (RH, row-vector, Z-up)
// =====================
class UCamera
{
public:
    UCamera()
        : mEye(10.0f, 0.0f, 0.0f)
        , mRight(0.0f, 1.0f, 0.0f)   // 보기 좋은 기본축 (X 왼/오, Y 앞/뒤, Z 위/아래 느낌)
        , mUp(0.0f, 0.0f, 1.0f)
        , mForward(-1.0f, 0.0f, 0.0f) // 화면 바라보는 방향(카메라 로컬 +Y 대신, 예시로 -X)
        , mFovYRad(ToRad(60.0f))
        , mAspect(1.0f)
        , mNearZ(0.1f)
        , mFarZ(1000.0f)
        , mUseOrtho(false)
        , mOrthoWidth(10.0f)
        , mOrthoHeight(10.0f)
    {
        Orthonormalize();
        RebuildView();
        RebuildProj();
    }

    // ===== 투영 설정 =====
    void SetPerspectiveDegrees(float fovYDeg, float aspect, float zn, float zf)
    {
        mUseOrtho = false;
        mFovYRad = ToRad(fovYDeg);
        mAspect = (aspect > 0.0f) ? aspect : 1.0f;
        mNearZ = zn; mFarZ = zf;
        RebuildProj();
    }
    void SetPerspective(float fovYRad, float aspect, float zn, float zf)
    {
        mUseOrtho = false;
        mFovYRad = fovYRad;
        mAspect = (aspect > 0.0f) ? aspect : 1.0f;
        mNearZ = zn; mFarZ = zf;
        RebuildProj();
    }
    // w,h는 뷰 공간 가로/세로 범위(직교 박스 크기)
    void SetOrtho(float w, float h, float zn, float zf, bool leftHanded=false)
    {
        mUseOrtho = true;
        mOrthoWidth = (w > 0.0f) ? w : 1.0f;
        mOrthoHeight = (h > 0.0f) ? h : 1.0f;
        mNearZ = zn; mFarZ = zf;
        RebuildProj(leftHanded);
    }
    void SetAspect(float aspect) { mAspect = (aspect > 0.0f) ? aspect : 1.0f; RebuildProj(); }

    // ===== 위치/자세 =====
    void SetPosition(const FVector& eye) { mEye = eye; RebuildView(); }
    const FVector& GetPosition() const { return mEye; }

    // target을 바라보도록 (RH, Z-up)
    void LookAt(const FVector& eye, const FVector& target, const FVector& up = FVector(0,0,1))
    {
        mEye = eye;
        // f = normalize(eye - target)  (RH에서 카메라 뷰행렬 구성 시 사용하는 f)
        FVector f = (eye - target).GetNormalized();
        FVector s = f.Cross(up).GetNormalized(); // right
        FVector u = s.Cross(f);                  // up'

        // 카메라 보조축으로 환원 (우리 내부 표현은 forward = -f)
        mRight = s;
        mUp    = u;
        mForward = (-f).GetNormalized();

        Orthonormalize();
        RebuildView();
    }

    // 로컬축 기준 이동 (dx=right, dy=forward, dz=up)
    void MoveLocal(float dx, float dy, float dz, float deltaTime, bool boost=false,
                   float baseSpeed=3.0f, float boostMul=3.0f)
    {
        float speed = baseSpeed * (boost ? boostMul : 1.0f);
        FVector delta = (mRight * dx + mForward * dy + mUp * dz) * (speed * deltaTime);
        mEye = mEye + delta;
        RebuildView();
    }

    // 회전: yawZ는 세계 Z축 기준, pitch는 카메라 right축 기준 (라디안)
    void AddYawPitch(float yawZ, float pitch)
    {
        if (yawZ != 0.0f) {
            RotateAroundWorldZ(yawZ);
        }
        if (pitch != 0.0f) {
            RotateAroundRight(pitch);
        }
        Orthonormalize();
        RebuildView();
    }

    // ===== 행렬 Get =====
    const FMatrix& GetView() const { return mView; }
    const FMatrix& GetProj() const { return mProj; }

private:
    // ---- 내부 상태 ----
    FVector mEye;      // 월드 위치
    FVector mRight;    // 카메라 x축
    FVector mUp;       // 카메라 z축(로컬 위)
    FVector mForward;  // 카메라 바라보는 방향 (+forward)

    // 투영 파라미터
    float   mFovYRad;
    float   mAspect;
    float   mNearZ, mFarZ;
    bool    mUseOrtho;
    float   mOrthoWidth, mOrthoHeight;

    // 행렬
    FMatrix mView;
    FMatrix mProj;

    // ---- 유틸 ----
    static inline float ToRad(float d) { return d * (float)(3.14159265358979323846 / 180.0); }

    void Orthonormalize()
    {
        // mForward 정규화
        mForward = mForward.GetNormalized();

        // 세계 Z-up을 기준으로 오른쪽 축을 먼저 안정화
        FVector worldUp(0,0,1);
        // 특이 케이스(거의 평행) 처리
        float dp = fabsf(mForward.Dot(worldUp));
        if (dp > 0.999f) {
            // forward가 거의 Z와 평행이면 다른 up을 임시로 사용
            worldUp = (fabsf(mForward.X) > 0.1f) ? FVector(0,1,0) : FVector(1,0,0);
        }

        mRight = mForward.Cross(worldUp).GetNormalized();
        mUp = mRight.Cross(mForward).GetNormalized();
    }

    // 세계 Z축 기준 yaw 회전
    void RotateAroundWorldZ(float yaw)
    {
        FMatrix R = FMatrix::RotationAxisRow(FVector(0,0,1), yaw);
        mRight   = R.TransformVectorRow(mRight);
        mUp      = R.TransformVectorRow(mUp);
        mForward = R.TransformVectorRow(mForward);
    }

    // 카메라 right축 기준 pitch 회전
    void RotateAroundRight(float pitch)
    {
        FMatrix R = FMatrix::RotationAxisRow(mRight, pitch);
        mUp      = R.TransformVectorRow(mUp);
        mForward = R.TransformVectorRow(mForward);
    }

    void RebuildView()
    {
        // FMatrix::LookAtRHRow와 동일 규약으로 구성
        // f = (eye - target) = -forward  -> forward = -f
        FVector f = (-mForward).GetNormalized();
        FVector s = mRight; // 이미 정규 직교
        FVector u = mUp;

        FMatrix V = FMatrix::IdentityMatrix();
        // 회전 성분 (열에 s,u,f를 배치 – 행벡터 규약)
        V.M[0][0] = s.X; V.M[0][1] = u.X; V.M[0][2] = f.X;
        V.M[1][0] = s.Y; V.M[1][1] = u.Y; V.M[1][2] = f.Y;
        V.M[2][0] = s.Z; V.M[2][1] = u.Z; V.M[2][2] = f.Z;

        // 평행이동 (마지막 행)
        V.M[3][0] = -(mEye.Dot(s));
        V.M[3][1] = -(mEye.Dot(u));
        V.M[3][2] = -(mEye.Dot(f));
        V.M[3][3] = 1.0f;

        mView = V;
    }

    void RebuildProj(bool leftHanded=false)
    {
        if (!mUseOrtho) {
            // RH + row-vector + D3D depth [0,1]
            mProj = FMatrix::PerspectiveFovRHRow(mFovYRad, mAspect, mNearZ, mFarZ);
        } else {
            // 직교 (요청 시 LH도 지원)
            if (!leftHanded) mProj = FMatrix::OrthoRHRow(mOrthoWidth, mOrthoHeight, mNearZ, mFarZ);
            else             mProj = FMatrix::OrthoLHRow(mOrthoWidth, mOrthoHeight, mNearZ, mFarZ);
        }
    }
};
