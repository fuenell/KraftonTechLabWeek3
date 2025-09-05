#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "SimpleApplication.h"
#include "UMeshFactory.h"
#include "ImguiConsole.h"

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
    static float t = 0.0f; t += 0.016f;

    // 구 위치를 시간에 따라 변경
    sphere->SetPosition({ 0.0f,  0.1f * t, 0.1f * t });

    int w = 0, h = 0; GetRenderer().GetBackBufferSize(w, h);
    float aspect = (h > 0) ? (float)w / (float)h : 1.0f;

    // RH + row 규약 투영
    FMatrix P = FMatrix::PerspectiveFovLHRow((float)(PIDIV4 / 4.0), aspect, 0.1f, 100.0f);

    // 카메라(궤도 이동 예시)
    float radius = 5.0f;
    FVector target(0.0f, 0.0f, 0.0f);
    FVector up(0, 0, 1);                 // Z-up
    // X축 방향 직선 이동
    //FVector eye(t, 0, 0.0f);
    // 타겟 = 현재 위치 + 바라보는 방향
    //FMatrix V = FMatrix::LookAtRHRow(eye, target, up);

    //카메라 위치: X축으로 직선 이동
    FVector eye(10.f, 0.0f, 0.0f);

    // 시선 고정: -X 방향을 계속 본다 ( Z-up)
    FVector forward(-1.0f, 0.0f, 0.0f);

    // 타겟 = 현재 위치 + 고정된 바라보는 방향
    //FVector target = eye + forward;
    FMatrix V = FMatrix::LookAtLHRow(eye, target, up);
    // Basic rendering - nothing for now
    // 3D objects would be rendered here
        // 구 그리기
    GetRenderer().SetViewProj(V, P);  // 카메라 행렬 세팅
}

void SimpleApplication::Render() 
{

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

    bool isOrthogonal = false;
    ImGui::Checkbox("Orthogonal", &isOrthogonal);

    // 데이터
    float fov = 53.000f;
    float cameraLocation[3] = { 1.575f, 2.509f, -1.599f };
    float cameraRotation[3] = { 0.820f, -0.458f, 0.000f };

    // FOV 행 - 테이블 밖에서 처리
    float tableWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetNextItemWidth(tableWidth * 0.75f); // 3/4 너비
    ImGui::InputFloat("##fov", &fov, 0.0f, 0.0f, "%.3f");
    ImGui::SameLine();
    ImGui::Text("FOV");

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
    // Factory에서 공유 Mesh 생성
    UMesh* sharedSphereMesh = UMeshFactory::CreateSphereMesh(GetRenderer());

    // Sphere 인스턴스 생성
    sphere = new USphereComp({ 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f }, sharedSphereMesh);
    sphere2 = new USphereComp({ 0.3f, 0.3f, 0.3f }, { 0.2f, 0.2f, 0.2f }, sharedSphereMesh);

    return true;
}