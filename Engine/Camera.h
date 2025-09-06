#pragma once
#include "stdafx.h"
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
// =====================
// UCamera (RH, row-vector, Z-up)
// =====================


class UCamera
{
public:
    UCamera()
        : mEye(0.0f, 0.0f, 0.0f)
        , mFovYRad(ToRad(60.0f))
        , mAspect(1.0f)
        , mNearZ(0.1f)
        , mFarZ(1000.0f)
        , mUseOrtho(false)
        , mOrthoWidth(10.0f)
        , mOrthoHeight(10.0f)
        , mRot(FQuaternion::Identity())       // ★ 쿼터니언 초기화
        , mPitch(0.0f)
    {
        RecalcAxesFromQuat();                 // ★ 축 유도
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

        // +Y forward 규약: 월드 forward = (target - eye)
        FVector f = (target - eye).GetNormalized();

        mRot = FQuaternion::LookRotation(f, up);

        // pitch 추정도 +Y 기준
        FVector fw = mRot.Rotate(FVector(0, 1, 0));
        float clampedZ = std::clamp(fw.Z, -1.0f, 1.0f);
        mPitch = asinf(clampedZ);

        RecalcAxesFromQuat();
        RebuildView();
    }

    // 로컬축 기준 이동 (dx=right, dy=forward, dz=up)
    void MoveLocal(float dx, float dy, float dz, float deltaTime, bool boost=false,
                   float baseSpeed=3.0f, float boostMul=3.0f)
    {
        RecalcAxesFromQuat(); // 안전: 최신 회전 반영
        float speed = baseSpeed * (boost ? boostMul : 1.0f);
        FVector delta = (mRight * dx + mForward * dy + mUp * dz) * (speed * deltaTime);
        mEye = mEye + delta;
        RebuildView();
    }

    // 회전: yawZ는 세계 Z축 기준, pitch는 카메라 right축 기준 (라디안)
    void AddYawPitch(float yawZ, float pitch)
    {
        // 1) Yaw (world Z)
        if (yawZ != 0.0f) {
            FQuaternion qYaw = FQuaternion::FromAxisAngle(FVector(0, 0, 1), yawZ);
            // ★ 세계 회전은 왼쪽 곱
            mRot = qYaw * mRot;
        }
        // 2) Pitch (current right)
        if (pitch != 0.0f) {
            const float kMax = ToRad(89.0f);
            float newPitch = std::clamp(mPitch + pitch, -kMax, kMax);
            float dp = newPitch - mPitch;
            if (fabsf(dp) > 1e-6f) {
                FVector right = mRot.Rotate(FVector(1, 0, 0)).GetNormalized();
                FQuaternion qPitch = FQuaternion::FromAxisAngle(right, dp);
                // ★ 로컬 회전은 오른쪽 곱
                mRot = mRot * qPitch;
                mPitch = newPitch;
            }
        }
        RecalcAxesFromQuat();
        RebuildView();
    }

    // ===== 행렬 Get =====
    const FMatrix& GetView() const { return mView; }
    const FMatrix& GetProj() const { return mProj; }



    float GetFovYDegrees() const { return mFovYRad * (180.0f / PI); }
    void  SetFovYDegrees(float deg) { SetPerspectiveDegrees(deg, mAspect, mNearZ, mFarZ); }

    float GetAspect() const { return mAspect; }
    float GetNearZ() const { return mNearZ; }
    float GetFarZ()  const { return mFarZ; }
    bool  IsOrtho()  const { return mUseOrtho; }

    const FVector& GetRight()   const { return mRight; }
    const FVector& GetUp()      const { return mUp; }
    const FVector& GetForward() const { return mForward; }

    // GUI 표시/편집용: Yaw(세계 Z), Pitch(카메라 Right)
    void GetYawPitch(float& yawZ, float& pitch) const {
        // forward로부터 구면좌표 (Z-up)
        FVector f = mForward;
        float clampedZ = (f.Z<-1.f? -1.f: (f.Z>1.f? 1.f: f.Z));
        pitch = asinf(clampedZ);
        yawZ = atan2f(f.Y, f.X);
        if (yawZ > PI) yawZ -= 2.f * PI;
        if (yawZ < -PI) yawZ += 2.f * PI;
    }
    void SetYawPitch(float yawZ, float pitch) {
        const float kMax = ToRad(89.0f);
        if (pitch > kMax) pitch = kMax;
        if (pitch < -kMax) pitch = -kMax;

        // 1) 절대 yaw: 세계 Z축 기준
        FQuaternion qYaw = FQuaternion::FromAxisAngle(FVector(0, 0, 1), yawZ);

        // 2) pitch: yaw 적용 후의 카메라 Right(로컬 X) 기준
        FVector right = qYaw.Rotate(FVector(1, 0, 0)).GetNormalized();
        FQuaternion qPitch = FQuaternion::FromAxisAngle(right, pitch);

        // 합성: 세계 yaw(왼쪽 곱) → 로컬 pitch(오른쪽 곱)
        mRot = qYaw * qPitch;
        mPitch = pitch;

        RecalcAxesFromQuat();
        RebuildView();
    }
private:
    // ---- 내부 상태 ----
    FVector mEye;      // 월드 위치
    FQuaternion mRot;  // ★ 회전(단일 진실원천)

    // 파생(읽기전용 캐시) — 매번 mRot에서 유도
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
	// ---- 회전 상태 ----
    // UCamera private 멤버에 누적 피치 저장
    float mPitch;

    // ---- 유틸 ----
    static inline float ToRad(float d) { return d * (float)(PI / 180.0); }

    // 쿼터니언 → 축 유도 (row 규약에서 로컬 축 정의: +X Right, +Y Forward, +Z Up)
    void RecalcAxesFromQuat() {
        mRight = mRot.Rotate(FVector(1, 0, 0)).GetNormalized();
        mForward = mRot.Rotate(FVector(0, 1, 0)).GetNormalized();
        mUp = mRot.Rotate(FVector(0, 0, 1)).GetNormalized();
        // 특이치 보정(거의 평행 등)은 필요 시 여기서 수행
    }

    void RebuildView() {
        mView = MakeViewRow(mEye, mRot);  // ★ T(-eye) * R^T
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

// 카메라를 ‘항상 Z-up’으로 유지해 롤 드리프트 제거
    void StabilizeUpright()
    {
        const FVector worldUp(0, 0, 1);

        // 현재 forward(+Y 로컬)를 월드로
        FVector f = mRot.Rotate(FVector(0, 1, 0)).GetNormalized();

        // forward가 worldUp과 거의 평행이면 보조 up 사용
        FVector upRef = (fabsf(f.Dot(worldUp)) > 0.999f) ? FVector(1, 0, 0) : worldUp;

        // RH: Right = f × upRef,  Up = Right × f
        FVector r = f.Cross(upRef); r.Normalize();
        FVector u = r.Cross(f);    // 자동 직교

        // 열: [r, f, u]
        FMatrix R = FMatrix::IdentityMatrix();
        R.M[0][0] = r.X; R.M[0][1] = f.X; R.M[0][2] = u.X;
        R.M[1][0] = r.Y; R.M[1][1] = f.Y; R.M[1][2] = u.Y;
        R.M[2][0] = r.Z; R.M[2][1] = f.Z; R.M[2][2] = u.Z;

        mRot = FQuaternion::FromMatrixRow(R); // 롤 제거
        RecalcAxesFromQuat();
    }
};
