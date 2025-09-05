#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "SimpleApplication.h"
#include "UMeshFactory.h"
#include "Camera.h"

const float PI = 3.14159265358979323846f;
const float PIDIV4 = PI / 4.0f;   // XM_PIDIV4 대체
// Simple application that inherits from UApplication
void SimpleApplication::Update(float deltaTime)
{
    // Basic update logic
    UApplication::Update(deltaTime);

    GetRenderer().GetBackBufferSize(width, height);
    //Camera.SetPerspectiveDegrees(60.0f, (height > 0) ? (float)width / (float)height : 1.0f, 0.1f, 100.0f);
    float dx = 0, dy = 0, dz = 0;
    bool boost = GetInputManager().IsKeyDown(VK_SHIFT); // Shift로 가속
    // Exit on ESC key
    if (GetInputManager().IsKeyDown(VK_ESCAPE))
    {
        RequestExit();
    }
    if(GetInputManager().IsKeyDown('W'))
    {
        dy += 1.0f; // 전진
	}
    if (GetInputManager().IsKeyDown('A'))
    {
        dx -= 1.0f; // 좌
    }
    if (GetInputManager().IsKeyDown('S'))
    {
        dy -= 1.0f; // 후진
    }
    if (GetInputManager().IsKeyDown('D'))
    {
        dx += 1.0f; // 우
    }
    if(GetInputManager().IsKeyDown('Q'))
    {
        dz += 1.0f; // 상
	}
    if (GetInputManager().IsKeyDown('E'))
    {
        dz -= 1.0f; // 하
	}
    camera.MoveLocal(dx, dy, dz, deltaTime, boost);
    static float t = 0.0f; t += 0.016f;
    // 대각선 이동 속도 보정(선택): 벡터 정규화
    float len = sqrtf(dx * dx + dy * dy + dz * dz);
    if (len > 0.f) { dx /= len; dy /= len; dz /= len; }
   /* Camera.MoveLocal(dx, dy, dz, deltaTime, boost);*/


    // Basic rendering - nothing for now
    // 3D objects would be rendered here
        // 구 그리기
    //GetRenderer().SetViewProj(Camera.GetView(), Camera.GetProj());  // 카메라 행렬 세팅
    sphere->SetPosition({ 0, 0.0f, 0.1f * t });
}

void SimpleApplication::Render() 
{
//    int w = 0, h = 0; GetRenderer().GetBackBufferSize(w, h);
//    float aspect = (h > 0) ? (float)w / (float)h : 1.0f;
//
//    // RH + row 규약 투영
//    FMatrix P = FMatrix::PerspectiveFovRHRow((float)(PIDIV4 / 4.0), aspect, 0.1f, 100.0f);
//
//    // 카메라(궤도 이동 예시)
//    FVector target(0.0f, 0.0f, 0.0f);
//    //FVector up(0, 0, 1);                 // Z-up
//    FVector eye(10.0f, 0.0f, 0.0f);
//    // X축 방향 직선 이동
//    //FVector eye(t, 0, 0.0f);
//    // 타겟 = 현재 위치 + 바라보는 방향
//    //FMatrix V = FMatrix::LookAtRHRow(eye, target, up);
//
//// 카메라 위치: X축으로 직선 이동
//    //FVector eye(t, 0.0f, 0.0f);
//
//    // 시선 고정: -X 방향을 계속 본다 (RH, Z-up)
//    FVector forward(0.0f, 0.0f, 0.0f);
//
//    // 타겟 = 현재 위치 + 고정된 바라보는 방향
//    //FVector target = eye + forward;
//    FVector up(0, 0, 1); // Z-up
//    FMatrix V = FMatrix::LookAtRHRow(eye, target, up);
    GetRenderer().SetViewProj(camera.GetView(), camera.GetProj());
    sphere->Draw(GetRenderer());
}

bool SimpleApplication::OnInitialize()
{
    // 리사이즈/초기화
    width = 0.0f;
	height = 0.0f;
	camera = UCamera();
    camera.SetPerspectiveDegrees(60.0f, (height > 0) ? (float)width / height : 1.0f, 0.1f, 1000.0f);
    camera.LookAt({ 5,0,0 }, { 0,0,0 }, { 0,0,1 });
    // Factory에서 공유 Mesh 생성
    UMesh* sharedSphereMesh = UMeshFactory::CreateSphereMesh(GetRenderer());

    // Sphere 인스턴스 생성
    sphere = new USphere({ 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f }, sharedSphereMesh);

    return true;
}