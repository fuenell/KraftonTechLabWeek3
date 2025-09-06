#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "SimpleApplication.h"
#include "UMeshFactory.h"
#include "Camera.h"
#include "ImguiConsole.h"

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
    // --- 마우스룩: RMB 누른 동안 회전 ---
    if (GetInputManager().IsMouseLooking()) {
        // 마우스룩 모드는 WndProc에서 Begin/End로 관리
        float mdx = 0.f, mdy = 0.f;
        GetInputManager().ConsumeMouseDelta(mdx, mdy);

        const float sens = 0.005f; // 일단 크게 해서 동작 확인
        camera.AddYawPitch(mdx * sens, mdy * sens);
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
    sphere->SetPosition({ 0, 0.0f, 0.1f * t });
}

void SimpleApplication::Render() 
{
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