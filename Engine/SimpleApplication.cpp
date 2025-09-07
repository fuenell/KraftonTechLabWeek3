#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "SimpleApplication.h"
#include "UMeshFactory.h"
#include "Camera.h"
#include "ImguiConsole.h"

// Simple application that inherits from UApplication
void SimpleApplication::Update(float deltaTime)
{
    // Basic update logic
    UApplication::Update(deltaTime);

    GetRenderer().GetBackBufferSize(width, height);
    if (height > 0) {
        camera.SetAspect((float)width / (float)height);
    }

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

        // 프레임 독립 감도: (기본감도 * deltaTime)
        const float baseSens = 1.5f; // 취향껏
        const float yawPerPx = baseSens * deltaTime;  // 좌우
        const float pitchPerPx = baseSens * deltaTime;  // 상하

        // Z-up + RH + row 에 맞춘 Yaw(세계 Z), Pitch(카메라 Right)
        //camera.AddYawPitch(mdx * yawPerPx, mdy * pitchPerPx);
    }

    // ====== 이동 입력 ======
    float dx = 0, dy = 0, dz = 0;
    if (GetInputManager().IsKeyDown('W')) dy += 1.0f;
    if (GetInputManager().IsKeyDown('S')) dy -= 1.0f;
    if (GetInputManager().IsKeyDown('D')) dx += 1.0f;
    if (GetInputManager().IsKeyDown('A')) dx -= 1.0f;
    if (GetInputManager().IsKeyDown('E')) dz += 1.0f;
    if (GetInputManager().IsKeyDown('Q')) dz -= 1.0f;


    // 대각선 이동 속도 보정(선택): 벡터 정규화
    float len = sqrtf(dx * dx + dy * dy + dz * dz);
    if (len > 0.f) { dx /= len; dy /= len; dz /= len; }

    bool boost = GetInputManager().IsKeyDown(VK_SHIFT);
    camera.MoveLocal(dx, dy, dz, deltaTime, boost);


    // Basic rendering - nothing for now
    // 3D objects would be rendered here
        // 구 그리기
    //GetRenderer().SetViewProj(Camera.GetView(), Camera.GetProj());  // 카메라 행렬 세팅
     static float t = 0.0f; t += 0.016f;
    sphere->SetPosition({ 0.1f * t,0 ,   0.0f });
    // 예: 초당 Yaw 30도 회전
    /*sphere->AddRotationEulerDeg(30.0f * deltaTime, 0.0f , 0.0f);*/
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

    // === 편집 UI ===
    bool locEdited = false, rotEdited = false;
    // 나머지는 테이블로
    if (ImGui::BeginTable("EditableCameraTable", 4, ImGuiTableFlags_None)) {
        // Camera Location 행
        ImGui::TableNextRow();
        for (int i = 0; i < 3; i++) {
            ImGui::TableSetColumnIndex(i);
            ImGui::SetNextItemWidth(-1);
            // InputFloat가 true면 그 필드가 수정된 것
            if (ImGui::InputFloat(("##loc" + std::to_string(i)).c_str(),
                &cameraLocation[i], 0.0f, 0.0f, "%.3f"))
            {
                locEdited = true;
            }
        }
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Camera Location");

        // Camera Rotation 행
        ImGui::TableNextRow();
        for (int i = 0; i < 3; i++) {
            ImGui::TableSetColumnIndex(i);
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputFloat(("##rot" + std::to_string(i)).c_str(),
                                   &cameraRotation[i], 0.0f, 0.0f, "%.3f"))
            {
                rotEdited = true;
            }
        }
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Camera Rotation");

        ImGui::EndTable();
    }

    // // === 변경되었을 때만 카메라에 반영 ===
    // 위치
    if (locEdited) {
        camera.SetPosition(FVector(cameraLocation[0], cameraLocation[1], cameraLocation[2]));
    }
    if (rotEdited) {
        // roll은 무시
        float newYawRad = cameraRotation[0] * 3.14159265f / 180.0f;
        float newPitchRad = cameraRotation[1] * 3.14159265f / 180.0f;
        //camera.SetYawPitch(newYawRad, newPitchRad);
    }
    // === Camera axes (world-space) 표시 ===
    {
        FVector r, f, u;
        camera.GetBasis(r, f, u);

        ImGui::Separator();
        ImGui::Text("Camera Axes (world-space)");
        if (ImGui::BeginTable("CamAxes", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Axis");
            ImGui::TableSetupColumn("X");
            ImGui::TableSetupColumn("Y");
            ImGui::TableSetupColumn("Z");
            ImGui::TableHeadersRow();

            auto Row = [&](const char* name, const FVector& v) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(name);
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", v.X);
                ImGui::TableSetColumnIndex(2); ImGui::Text("%.3f", v.Y);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.3f", v.Z);
                };

            Row("Right  (+X)", r);
            Row("Forward(+Y)", f);
            Row("Up     (+Z)", u);

            ImGui::EndTable();
        }
    }

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
    camera.LookAt({ 0, 5 ,0 }, { 0,0,0 }, { 0,0,1 });
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