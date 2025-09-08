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
        , mRot(FQuaternion::FromAxisAngle(FVector(0,0,1), PI/2))       // ★ 쿼터니언 초기화
        , mView(FMatrix::IdentityMatrix())
        , mProj(FMatrix::IdentityMatrix())
        , mFovY(ToRad(60.0f)), mAspect(1.0f), mNearZ(0.1f), mFarZ(1000.0f)
        , mUseOrtho(false)
        , mOrthoWidth(10.0f)
        , mOrthoHeight(10.0f)
        , bLockRoll(true)
    {
        UpdateView();
        UpdateProj();
    }
    // ===== 투영 설정 =====
    void SetPerspective(float fovY, float aspect, float zn, float zf) {
        mFovY = fovY; mAspect = aspect; mNearZ = zn; mFarZ = zf;
        mProj = FMatrix::PerspectiveFovRHRow(fovY, aspect, zn, zf);
    }
    void SetPerspectiveDegrees(float fovYDeg, float aspect, float zn, float zf)
    {
        mUseOrtho = false;
        mFovY = ToRad(fovYDeg);
        mAspect = (aspect > 0.0f) ? aspect : 1.0f;
        mNearZ = zn; mFarZ = zf;
        UpdateProj();
    }
    float GetFovYDegrees() const { return mFovY * (180.0f / 3.14159265358979323846f); }
    void  SetFovYDegrees(float deg) { SetPerspectiveDegrees(deg, mAspect, mNearZ, mFarZ); }
    // ===== 위치/자세 =====
    void SetPosition(const FVector& eye) { mEye = eye; UpdateView(); }
    void SetRotation(const FQuaternion& q) { mRot = q.Normalized(); UpdateView(); }
    void SetLockRoll(bool on) { bLockRoll = on; UpdateView(); }
    // target을 바라보도록 (RH, Z-up)
    void LookAt(const FVector& eye, const FVector& target, const FVector& up = FVector(0, 0, 1))
    {
        mEye = eye;
        FVector f = (target - eye).GetNormalized();
        if (f.Length() < 1e-6f) f = FVector(0, -1, 0); // 안전장치

        FQuaternion q = FQuaternion::LookRotation(f, up);
        if (bLockRoll) {
            // f 방향만 맞추고, 최종적으로 Up을 월드 Z와 일치시키는 간단 롤 보정
            // (필요 없으면 제거 가능)
            FVector fwd = q.Rotate(FVector(0, 1, 0)); // 모델 +Y
            FVector up = FVector(0, 0, 1); // 강제 업
            // r,u,f를 다시 정규직교화
            FVector r = (fwd.Cross(up)).GetNormalized();
            FVector u = (r.Cross(fwd)).GetNormalized();
            // r,u,f로 다시 쿼터니언 구성
            q = FQuaternion::LookRotation(fwd, u);
        }
        mRot = q.Normalized();
        UpdateView();
    }
    // w,h는 뷰 공간 가로/세로 범위(직교 박스 크기)
    void SetOrtho(float w, float h, float zn, float zf, bool leftHanded=false)
    {
        mUseOrtho = true;
        mOrthoWidth = (w > 0.0f) ? w : 1.0f;
        mOrthoHeight = (h > 0.0f) ? h : 1.0f;
        mNearZ = zn; mFarZ = zf;
        UpdateProj(leftHanded);
    }
    void SetAspect(float aspect) { mAspect = (aspect > 0.0f) ? aspect : 1.0f; UpdateProj(); }
    // 로컬축 기준 이동 (dx=right, dy=forward, dz=up)
    void MoveLocal(float dx, float dy, float dz, float deltaTime, bool boost=false,
                   float baseSpeed=3.0f, float boostMul=3.0f)
    {
        RecalcAxesFromQuat(); // 안전: 최신 회전 반영
        float speed = baseSpeed * (boost ? boostMul : 1.0f);
        FVector delta = (mRight * dx + mForward * dy + mUp * dz) * (speed * deltaTime);
        mEye = mEye + delta;
        UpdateView();
    }
    void AddYawPitch(float yawRad, float pitchRad)
    {
        // 로컬 축 기준으로 회전 (Yaw: Z-up에 대해, Pitch: Right에 대해)
        // 간단히: 현재 축을 얻어 쿼터니언 갱신
        FVector right = mRot.Rotate(FVector(1, 0, 0));
        FVector up = mRot.Rotate(FVector(0, 0, 1));
        // 1) Yaw (world Z)
        FQuaternion qYaw   = FQuaternion::FromAxisAngle(up, yawRad);
        FQuaternion qPitch = FQuaternion::FromAxisAngle(right, pitchRad);
        // qNew * qOld (벡터를 왼쪽에서 회전시키는 규약)
        mRot = (qPitch * (qYaw * mRot)).Normalized();  // 곱셈 연산자 있으면 이렇게 간결히
        if (bLockRoll) {
            // 롤 살짝 정리
            FVector fwd = mRot.Rotate(FVector(0,1,0));
            LookAt(mEye, mEye + fwd, FVector(0,0,1));
            return;
        }
    }

    // ===== 행렬 Get =====
    const FMatrix& GetView() const { return mView; }
    const FMatrix& GetProj() const { return mProj; }
    float GetAspect() const { return mAspect; }
    float GetNearZ() const { return mNearZ; }
    float GetFarZ()  const { return mFarZ; }
    bool  IsOrtho()  const { return mUseOrtho; }
    const FVector& GetPosition() const { return mEye; }
    const FVector& GetRight()   const { return mRight; }
    const FVector& GetUp()      const { return mUp; }
    const FVector& GetForward() const { return mForward; }
    void GetBasis(FVector& outRight, FVector& outForward, FVector& outUp) const {
        outRight = GetRight();
        outForward = GetForward();
        outUp = GetUp();
    }

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


        UpdateView();
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
    float   mFovY;
    float   mAspect;
    float   mNearZ, mFarZ;
    bool    mUseOrtho;
    float   mOrthoWidth, mOrthoHeight;

    // 행렬
    FMatrix mView;
    FMatrix mProj;
    bool  bLockRoll;
    // ---- 유틸 ----
    static inline float ToRad(float d) { return d * (float)(PI / 180.0); }

    // 쿼터니언 → 축 유도 (row 규약에서 로컬 축 정의: +X Right, -Y Forward, +Z Up)
	// 카메라는 -Y가 forward 방향
    void RecalcAxesFromQuat() {
        mRight = mRot.Rotate(FVector(1, 0, 0)).GetNormalized();
        mUp = mRot.Rotate(FVector(0, 0, 1)).GetNormalized();
        mForward = (mUp.Cross(mRight)).GetNormalized();
        // 특이치 보정(거의 평행 등)은 필요 시 여기서 수행
    }

    void UpdateView() {
        RecalcAxesFromQuat(); // GUI를 위해서
        mView = MakeViewRow(mEye, mRot);  // ★ T(-eye) * R^T
    }

    void UpdateProj(bool leftHanded=false)
    {
        if (!mUseOrtho) {
            // RH + row-vector + D3D depth [0,1]
            mProj = FMatrix::PerspectiveFovRHRow(mFovY, mAspect, mNearZ, mFarZ);
        } else {
            // 직교 (요청 시 LH도 지원)
            if (!leftHanded) mProj = FMatrix::OrthoRHRow(mOrthoWidth, mOrthoHeight, mNearZ, mFarZ);
            else             mProj = FMatrix::OrthoLHRow(mOrthoWidth, mOrthoHeight, mNearZ, mFarZ);
        }
    }
};
