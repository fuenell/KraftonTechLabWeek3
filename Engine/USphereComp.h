#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "FVertexPosColor.h"
#include "Vector.h"
#include "Quaternion.h"

class USphereComp : public UPrimitiveComponent
{
private:
	// 임시로 위치와 스케일 저장
	// 내부 회전은 쿼터니언이 진실원천(SOT)
	FQuaternion Rotation = FQuaternion::Identity();
	// 회전도 원하면 유지
    float   yaw = 0, pitch = 0, roll = 0;

public:
	USphereComp(FVector pos = { 0, 0, 0 }, FVector scl = { 1, 1, 1 }, UMesh* sphereMesh = nullptr)
	{
		mesh = sphereMesh;
		RelativeLocation = pos;
		RelativeScale3D = scl;
	}

	void UpdatePhysics(float t, bool bUsingGravity, float restitution) override;
	bool OnCollision(UPrimitiveComponent* other, float restitution) override;

    void UpdateConstantBuffer(URenderer& renderer) override;
	void Draw(URenderer& renderer) override;

	// 위치와 스케일 설정 함수들
	void SetPosition(const FVector& pos) { RelativeLocation = pos; }
	void SetScale(const FVector& scl) { RelativeScale3D = scl; }
	FVector GetPosition() const { return RelativeLocation; }
	FVector GetScale() const { return RelativeScale3D; }

	// 임시 ==========================

    // ===== 회전 API (쿼터니언 SOT) =====
    void SetRotationQuat(const FQuaternion& q) { Rotation = q.Normalized(); }
    FQuaternion GetRotationQuat() const { return Rotation; }

    // 오일러 (라디안)
    void SetRotationEulerRad(float pitch, float yaw, float roll);
    void GetRotationEulerRad(float& pitch, float& yaw, float& roll) const;

    // 오일러 (도) – GUI용
    void SetRotationEulerDeg(float pitchDeg, float yawDeg, float rollDeg);
    void GetRotationEulerDeg(float& pitchDeg, float& yawDeg, float& rollDeg) const;

    // 증분(라디안/도)
    void AddRotationEulerRad(float dp, float dy, float dr);
    void AddRotationEulerDeg(float dpDeg, float dyDeg, float drDeg);

private:
    static inline float ToRad(float d) { return d * (float)(PI / 180.0f); }
    static inline float ToDeg(float r) { return r * (float)(180.0f / PI); }
};