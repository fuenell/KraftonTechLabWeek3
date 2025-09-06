#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "SimpleApplication.h"
#include "UMeshFactory.h"
#include "Camera.h"
#include "ImguiConsole.h"
#include "UScene.h"

const float PI = 3.14159265358979323846f;
const float PIDIV4 = PI / 4.0f;   // XM_PIDIV4 대체
// Simple application that inherits from UApplication
void SimpleApplication::Update(float deltaTime)
{
    // Basic update logic
    UApplication::Update(deltaTime);

    GetRenderer().GetBackBufferSize(width, height);
    if (height > 0) {
        camera.SetAspect((float)width / (float)height);
    }
    float dx = 0, dy = 0, dz = 0;
    bool boost = GetInputManager().IsKeyDown(VK_SHIFT); // Shift로 가속
    // Exit on ESC key
    if (GetInputManager().IsKeyDown(VK_ESCAPE))
    {
        RequestExit();
    }
    // --- 마우스룩: RMB 누른 동안 회전 ---
    if (GetInputManager().IsMouseLooking()) {
        // 마우스룩 모드는 WndProc에서 Begin/End로 관리
        float mdx = 0.f, mdy = 0.f;
        GetInputManager().ConsumeMouseDelta(mdx, mdy);

        const float sens = 0.005f; // 일단 크게 해서 동작 확인
        camera.AddYawPitch(-mdx * sens, -mdy * sens);
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
    static float t = 0.0f; t += 0.016f;
    // 대각선 이동 속도 보정(선택): 벡터 정규화
    float len = sqrtf(dx * dx + dy * dy + dz * dz);
    if (len > 0.f) { dx /= len; dy /= len; dz /= len; }
    camera.MoveLocal(dx, dy, dz, deltaTime, boost);


    // Basic rendering - nothing for now
    // 3D objects would be rendered here
        // 구 그리기
    //GetRenderer().SetViewProj(Camera.GetView(), Camera.GetProj());  // 카메라 행렬 세팅
    //sphere->SetPosition({ 0, 0.0f, 0.1f * t });
}

void SimpleApplication::Render() 
{
    // 카메라가 바뀌면 원하는 타이밍(매 프레임도 OK)에 알려주면 됨
    GetRenderer().SetTargetAspect(camera.GetAspect());

    GetRenderer().SetViewProj(camera.GetView(), camera.GetProj());
    sphere->Draw(GetRenderer());
    sphere2->Draw(GetRenderer());
}

void SimpleApplication::RenderGUI()
{
    ImGui::Begin("Jungle Control Panel");

    ImGui::Text("Hello Jungle World!");

    float frameRate = ImGui::GetIO().Framerate;
    ImGui::Text("FPS %.0f (%.0f ms)", frameRate,1000.0f/frameRate);

    ImGui::Separator();

    static int currentItem = 0;
    int value = 10;
    const char* choices[] = {
        "a", "b"
    };


    ImGui::Combo("Primitive", &currentItem, choices, sizeof(choices) / sizeof(const char*));
    ImGui::Button("Spawn");
    ImGui::SameLine();

    ImGui::BeginDisabled();
    ImGui::SameLine();
    ImGui::InputInt("Number of spawned primitives", &value, 0);
    ImGui::EndDisabled();

    ImGui::Separator();

    char sceneName[100] = "";
    ImGui::InputText("Scene Name", sceneName, sizeof(sceneName));
    ImGui::Button("New scene");
    ImGui::Button("Save scene");
    ImGui::Button("Load scene");

    ImGui::Separator();

    bool isOrthogonal = camera.IsOrtho();
    ImGui::Checkbox("Orthogonal", &isOrthogonal);
    if (isOrthogonal) {
        // 원하는 직교 크기로 (예시: 월드 단위 10x10)
        camera.SetOrtho(10.0f, 10.0f, camera.GetNearZ(), camera.GetFarZ(), /*leftHanded=*/false);
    }
    else {
        camera.SetPerspectiveDegrees(camera.GetFovYDegrees(),
            camera.GetAspect(), camera.GetNearZ(), camera.GetFarZ());
    }


    // === FOV (perspective일 때만 활성화) ===
    float fovDeg = camera.GetFovYDegrees();
    float tableWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetNextItemWidth(tableWidth * 0.75f);
    ImGui::BeginDisabled(isOrthogonal);
    if (ImGui::InputFloat("##fov", &fovDeg, 0.0f, 0.0f, "%.3f")) {
        camera.SetFovYDegrees(fovDeg); // proj 재빌드 내부에서 함
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::Text("FOV");

    // === 위치 ===
    FVector pos = camera.GetPosition();
    float cameraLocation[3] = { pos.X, pos.Y, pos.Z };

    // === 회전(Yaw, Pitch, Roll=0 표기) ===
    float yawZ = 0.f, pitch = 0.f;
    camera.GetYawPitch(yawZ, pitch);
    float cameraRotation[3] = {
        yawZ * 180.0f / 3.14159265f,   // deg
        pitch * 180.0f / 3.14159265f,  // deg
        0.0f                           // roll 고정
    };

    // 나머지는 테이블로
    if (ImGui::BeginTable("EditableCameraTable", 4, ImGuiTableFlags_None)) {
        // Camera Location 행
        ImGui::TableNextRow();
        for (int i = 0; i < 3; i++) {
            ImGui::TableSetColumnIndex(i);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat(("##loc" + std::to_string(i)).c_str(),
                &cameraLocation[i], 0.0f, 0.0f, "%.3f");
        }
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Camera Location");

        // Camera Rotation 행
        ImGui::TableNextRow();
        for (int i = 0; i < 3; i++) {
            ImGui::TableSetColumnIndex(i);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat(("##rot" + std::to_string(i)).c_str(),
                &cameraRotation[i], 0.0f, 0.0f, "%.3f");
        }
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Camera Rotation");

        ImGui::EndTable();
    }

    // === 변경사항을 카메라에 반영 ===
    // 위치
    camera.SetPosition(FVector(cameraLocation[0], cameraLocation[1], cameraLocation[2]));

    // 회전 (roll은 무시)
    float newYawRad = cameraRotation[0] * 3.14159265f / 180.0f;
    float newPitchRad = cameraRotation[1] * 3.14159265f / 180.0f;
    camera.SetYawPitch(newYawRad, newPitchRad);

    ImGui::End();

    ImGui::Begin("Jungle Property Window");

    // 나머지는 테이블로
    if (ImGui::BeginTable("EditablePropertyTable", 4, ImGuiTableFlags_None)) {
        ImGui::TableNextRow();
        for (int i = 0; i < 3; i++) {
            ImGui::TableSetColumnIndex(i);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat(("##tra" + std::to_string(i)).c_str(),
                &cameraLocation[i], 0.0f, 0.0f, "%.3f");
        }
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Translation");

        ImGui::TableNextRow();
        for (int i = 0; i < 3; i++) {
            ImGui::TableSetColumnIndex(i);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat(("##rot" + std::to_string(i)).c_str(),
                &cameraRotation[i], 0.0f, 0.0f, "%.3f");
        }
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Rotation");

        ImGui::TableNextRow();
        for (int i = 0; i < 3; i++) {
            ImGui::TableSetColumnIndex(i);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat(("##scl" + std::to_string(i)).c_str(),
                &cameraRotation[i], 0.0f, 0.0f, "%.3f");
        }
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Scale");

        ImGui::EndTable();
    }

    ImGui::End();

    bool isConsoleOpen = false;
    static ImguiConsole imguiConsole;
    imguiConsole.Draw("Console", &isConsoleOpen);
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


    UMesh* gridMesh = UMeshFactory::CreateGizmoGridMesh(GetRenderer());

    // Sphere 인스턴스 생성
    sphere = new USphereComp({ 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f }, sharedSphereMesh);
    sphere2 = new USphereComp({ 0.3f, 0.3f, 0.3f }, { 0.2f, 0.2f, 0.2f }, gridMesh);

    return true;
}


void SimpleApplication::OnResize(int width, int height) {
    camera.SetPerspectiveDegrees(
        camera.GetFovYDegrees(),
        (height > 0) ? (float)width / (float)height : 1.0f,
        camera.GetNearZ(),
        camera.GetFarZ());
}