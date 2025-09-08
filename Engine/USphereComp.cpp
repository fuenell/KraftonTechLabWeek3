#include "stdafx.h"
#include "USphereComp.h"

void USphereComp::UpdatePhysics(float t, bool bUsingGravity, float restitution)
{
    // 물리 업데이트 로직 (나중에 구현)
}

bool USphereComp::OnCollision(UPrimitiveComponent* other, float restitution)
{
    // 충돌 처리 (나중에 구현)
    return false;
}

void USphereComp::UpdateConstantBuffer(URenderer& renderer)
{
    //FMatrix M = FMatrix::TRSRow(RelativeLocation, FVector(pitch, yaw, roll), RelativeScale3D);
    // renderer.SetModel(M);          // M,V,P를 통째로 상수버퍼에 업로드
    // 쿼터니언 기반 모델행렬: M = S * R(q) * T
    FMatrix M = MakeModelRow(RelativeLocation, Rotation, RelativeScale3D);
    renderer.SetModel(M);
}

void USphereComp::Draw(URenderer& renderer)
{
    if (!mesh || !mesh->VertexBuffer)
    {
        return;
    }

    UpdateConstantBuffer(renderer);
    renderer.DrawMesh(mesh);
}
// 여기는 임시========================================================
// ===== 오일러 <-> 쿼터니언 변환 =====
// 규약: intrinsic XYZ (pitch=X, yaw=Y, roll=Z) 순서 적용
void USphereComp::SetRotationEulerRad(float pitch, float yaw, float roll)
{
    Rotation = FQuaternion::FromEulerXYZ(pitch, yaw, roll); // 라디안
}

void USphereComp::GetRotationEulerRad(float& pitch, float& yaw, float& roll) const
{
    // 쿼터니언 -> 행렬 -> XYZ 오일러 추출 (row-vector 규약)
    FMatrix R = Rotation.ToMatrixRow();

    // 행렬 원소
    float m00 = R.M[0][0], m01 = R.M[0][1], m02 = R.M[0][2];
    float m10 = R.M[1][0], m11 = R.M[1][1], m12 = R.M[1][2];
    float m20 = R.M[2][0], m21 = R.M[2][1], m22 = R.M[2][2];

    // XYZ (pitch=X, yaw=Y, roll=Z) 추출
    // 참고: m20 = sin(pitch) *something 이 아니라, row 규약 상 다음이 안정적
    // 여기서는 전형적 XYZ 분해:
    // pitch = asin( clamp(m21, -1, 1) )
    // yaw   = atan2(-m20, m22)
    // roll  = atan2(-m01, m11)
    // 다만 구현/규약에 따라 약간씩 다르니 아래 공식을 사용
    float sp = m21;                   // ≈ sin(pitch)
    sp = (sp < -1.f) ? -1.f : (sp > 1.f ? 1.f : sp);
    pitch = asinf(sp);

    // Gimbal near ±90° 보호
    if (fabsf(sp) < 0.9999f) {
        yaw = atan2f(-m20, m22);
        roll = atan2f(-m01, m11);
    }
    else {
        // 특이점 근처: 대체 분해
        yaw = atan2f(m02, m00);
        roll = 0.0f;
    }
}

void USphereComp::SetRotationEulerDeg(float pitchDeg, float yawDeg, float rollDeg)
{
    SetRotationEulerRad(ToRad(pitchDeg), ToRad(yawDeg), ToRad(rollDeg));
}

void USphereComp::GetRotationEulerDeg(float& pitchDeg, float& yawDeg, float& rollDeg) const
{
    float pr, yr, rr;
    GetRotationEulerRad(pr, yr, rr);
    pitchDeg = ToDeg(pr);
    yawDeg = ToDeg(yr);
    rollDeg = ToDeg(rr);
}

// 증분
void USphereComp::AddRotationEulerRad(float dp, float dy, float dr)
{
    // 현재 로컬축을 "월드축"으로 뽑는다: ax = q·(+X), ay = q·(+Y), az = q·(+Z)
    FVector ax = Rotation.Rotate(FVector(1, 0, 0)).GetNormalized();
    FVector ay = Rotation.Rotate(FVector(0, 1, 0)).GetNormalized();
    FVector az = Rotation.Rotate(FVector(0, 0, 1)).GetNormalized();

    // 각 축 기준 +각도 회전 쿼터니언
    FQuaternion qx = FQuaternion::FromAxisAngle(ax, dp);
    FQuaternion qy = FQuaternion::FromAxisAngle(ay, dy);
    FQuaternion qz = FQuaternion::FromAxisAngle(az, dr);

    // ★ row-vector에서 "로컬 회전"은 왼쪽 곱으로 누적 (X → Y → Z 적용 순서)
    FQuaternion dq = qz * qy * qx;
    Rotation = (dq * Rotation).Normalized();
}

void USphereComp::AddRotationEulerDeg(float dpDeg, float dyDeg, float drDeg)
{
    AddRotationEulerRad(ToRad(dpDeg), ToRad(dyDeg), ToRad(drDeg));
}