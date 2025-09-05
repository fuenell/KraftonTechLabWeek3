#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "SimpleApplication.h"
#include "UMeshFactory.h"

const float PI = 3.14159265358979323846f;
const float PIDIV4 = PI / 4.0f;   // XM_PIDIV4 대체
// Simple application that inherits from UApplication
void SimpleApplication::Update(float deltaTime)
{
    // Basic update logic
    UApplication::Update(deltaTime);

    // Exit on ESC key
    if (GetInputManager().IsKeyPressed(VK_ESCAPE))
    {
        RequestExit();
    }

    // 구 회전 애니메이션 (선택사항)
    static float rotation = 0.0f;
    rotation += deltaTime * 1.0f; // 1 radian per second

    // 구 위치를 시간에 따라 변경
    sphere->SetPosition({ 0.0f, 0.0f, 0.0f });
    sphere2->SetPosition({ 0.0f, 0.0f, 0.0f });
}

void SimpleApplication::Render() 
{
    int w = 0, h = 0; GetRenderer().GetBackBufferSize(w, h);
    float aspect = (h > 0) ? (float)w / (float)h : 1.0f;

    // RH + row 규약 투영
    FMatrix P = FMatrix::PerspectiveFovRHRow((float)(PIDIV4 / 4.0), aspect, 0.1f, 100.0f);

    // 카메라(궤도 이동 예시)
    static float t = 0.0f; t += 0.016f;
    float radius = 5.0f;
    FVector target(cosf(t) * radius, sinf(t) * radius, 0.0f);
    //FVector up(0, 0, 1);                 // Z-up
    FVector eye(10.0f, 0.0f , 0.0f);
    // X축 방향 직선 이동
    //FVector eye(t, 0, 0.0f);
    // 타겟 = 현재 위치 + 바라보는 방향
    //FMatrix V = FMatrix::LookAtRHRow(eye, target, up);

// 카메라 위치: X축으로 직선 이동
    //FVector eye(t, 0.0f, 0.0f);

    // 시선 고정: -X 방향을 계속 본다 (RH, Z-up)
    FVector forward(-1.0f, 0.0f, 0.0f);

    // 타겟 = 현재 위치 + 고정된 바라보는 방향
    //FVector target = eye + forward;
    FVector up(0, 0, 1); // Z-up
    FMatrix V = FMatrix::LookAtRHRow(eye, target, up);
    // Basic rendering - nothing for now
    // 3D objects would be rendered here
        // 구 그리기
    GetRenderer().SetViewProj(V, P);  // 카메라 행렬 세팅
    sphere->Draw(GetRenderer());
}

bool SimpleApplication::OnInitialize()
{
    // Factory에서 공유 Mesh 생성
    UMesh* sharedSphereMesh = UMeshFactory::CreateSphereMesh(GetRenderer());

    // Sphere 인스턴스 생성
    sphere = new USphere({ 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f }, sharedSphereMesh);
    sphere2 = new USphere({ 0.3f, 0.3f, 0.3f }, { 0.2f, 0.2f, 0.2f }, sharedSphereMesh);

    return true;
}