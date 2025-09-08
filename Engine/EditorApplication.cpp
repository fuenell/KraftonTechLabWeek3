#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "EditorApplication.h"
#include "UMeshManager.h"
#include "ImguiConsole.h"
#include "UScene.h"
#include "UDefaultScene.h"
#include "URaycastManager.h"

// Simple application that inherits from UApplication
void EditorApplication::Update(float deltaTime)
{
    // Basic update logic
    UApplication::Update(deltaTime);

    UGizmoComponent* gizmoHit = nullptr;
    UPrimitiveComponent* sceneHit = nullptr;
    // 아무것도 선택되지 않음

    if (GetInputManager().IsMouseButtonDown(0))
    {
        gizmoManager.SetTarget(nullptr);
        if (gizmoManager.Raycast(&GetRaycastManager(), *gizmoHit))
        {
            // Gizmo가 선택되었으므로 여기서 처리 종료
            // SelectGizmoHandle(gizmoHit);
        }
        // 2순위: 게임 오브젝트
        else
        {
            TArray<UPrimitiveComponent*> primitives;

            for (UObject* obj : GetSceneManager().GetScene()->GetObjects())
            {
                if (UPrimitiveComponent* primitive = dynamic_cast<UPrimitiveComponent*>(obj))
                {
                    if (primitive->GetMesh())
                    {
                        primitives.push_back(primitive);
                    }
                }
            }

            GetRaycastManager().RayIntersectsMeshes(GetSceneManager().GetScene()->GetCamera(), primitives, sceneHit);

            for (UObject* object : GetSceneManager().GetScene()->GetObjects())
            {
                UPrimitiveComponent* primitive = dynamic_cast<UPrimitiveComponent*>(object);
                if (primitive) primitive->bIsSelected = false;
            }
            
            if (sceneHit)
            {
                gizmoManager.SetTarget(sceneHit);
                sceneHit->bIsSelected = true;
            }
        }
    }

    // Exit on ESC key
    if (GetInputManager().IsKeyDown(VK_ESCAPE))
    {
        RequestExit();
    }
}

void EditorApplication::Render()
{
    gizmoManager.Draw(GetRenderer());
    UApplication::Render();
}

void EditorApplication::RenderGUI()
{
    ImGui::Begin("Jungle Control Panel");

    ImGui::Text("Hello Jungle World!");

    float frameRate = ImGui::GetIO().Framerate;
    ImGui::Text("FPS %.0f (%.0f ms)", frameRate, 1000.0f / frameRate);

    ImGui::Separator();

    static int currentItem = 0;
    int value = GetSceneManager().GetScene()->GetObjectCount();
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

    static char sceneName[100] = "";
    ImGui::InputText("Scene Name", sceneName, sizeof(sceneName));
    ImGui::Button("New scene");
    if (ImGui::Button("Save scene"))
    {
        std::filesystem::path _path("./data/");
        std::filesystem::create_directory(_path);
        GetSceneManager().SaveScene(_path.string() + std::string(sceneName) + ".json");
    }
    if (ImGui::Button("Load scene"))
    {
        GetSceneManager().LoadScene("./data/" + std::string(sceneName) + ".json");
    }

    ImGui::Separator();

    UCamera* camera = GetSceneManager().GetScene()->GetCamera();

    bool isOrthogonal = camera->IsOrtho();
    ImGui::Checkbox("Orthogonal", &isOrthogonal);
    if (isOrthogonal)
    {
        // 원하는 직교 크기로 (예시: 월드 단위 10x10)
        camera->SetOrtho(10.0f, 10.0f, camera->GetNearZ(), camera->GetFarZ(), /*leftHanded=*/false);
    }
    else
    {
        camera->SetPerspectiveDegrees(camera->GetFovYDegrees(),
                                      camera->GetAspect(), camera->GetNearZ(), camera->GetFarZ());
    }


    // === FOV (perspective일 때만 활성화) ===
    float fovDeg = camera->GetFovYDegrees();
    float tableWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetNextItemWidth(tableWidth * 0.75f);
    ImGui::BeginDisabled(isOrthogonal);
    if (ImGui::InputFloat("##fov", &fovDeg, 0.0f, 0.0f, "%.3f"))
    {
        camera->SetFovYDegrees(fovDeg); // proj 재빌드 내부에서 함
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::Text("FOV");

    // === 위치 ===
    FVector pos = camera->GetPosition();
    float cameraLocation[3] = {pos.X, pos.Y, pos.Z};

    // === 회전(Yaw, Pitch, Roll=0 표기) ===
    float yawZ = 0.f, pitch = 0.f;
    camera->GetYawPitch(yawZ, pitch);
    float cameraRotation[3] = {
        yawZ * 180.0f / 3.14159265f, // deg
        pitch * 180.0f / 3.14159265f, // deg
        0.0f // roll 고정
    };

    // 나머지는 테이블로
    if (ImGui::BeginTable("EditableCameraTable", 4, ImGuiTableFlags_None))
    {
        // Camera Location 행
        ImGui::TableNextRow();
        for (int i = 0; i < 3; i++)
        {
            ImGui::TableSetColumnIndex(i);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat(("##loc" + std::to_string(i)).c_str(),
                              &cameraLocation[i], 0.0f, 0.0f, "%.3f");
        }
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Camera Location");

        // Camera Rotation 행
        ImGui::TableNextRow();
        for (int i = 0; i < 3; i++)
        {
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
    camera->SetPosition(FVector(cameraLocation[0], cameraLocation[1], cameraLocation[2]));

    // 회전 (roll은 무시)
    float newYawRad = cameraRotation[0] * 3.14159265f / 180.0f;
    float newPitchRad = cameraRotation[1] * 3.14159265f / 180.0f;
    camera->SetYawPitch(newYawRad, newPitchRad);

    ImGui::End();

    ImGui::Begin("Jungle Property Window");

    // 나머지는 테이블로
    if (ImGui::BeginTable("EditablePropertyTable", 4, ImGuiTableFlags_None))
    {
        ImGui::TableNextRow();
        for (int i = 0; i < 3; i++)
        {
            ImGui::TableSetColumnIndex(i);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat(("##tra" + std::to_string(i)).c_str(),
                              &cameraLocation[i], 0.0f, 0.0f, "%.3f");
        }
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Translation");

        ImGui::TableNextRow();
        for (int i = 0; i < 3; i++)
        {
            ImGui::TableSetColumnIndex(i);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat(("##rot" + std::to_string(i)).c_str(),
                              &cameraRotation[i], 0.0f, 0.0f, "%.3f");
        }
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Rotation");

        ImGui::TableNextRow();
        for (int i = 0; i < 3; i++)
        {
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


    ImGui::Begin("Memory Stats");
    ImGui::Text("Allocated Object Count : %d", UEngineStatics::GetTotalAllocationCount());
    ImGui::Text("Allocated Object Bytes : %d", UEngineStatics::GetTotalAllocationBytes());
    ImGui::End();

    bool isConsoleOpen = false;
    static ImguiConsole imguiConsole;
    imguiConsole.Draw("Console", &isConsoleOpen);
}

bool EditorApplication::OnInitialize()
{
    UApplication::OnInitialize();
    // 리사이즈/초기화

    if (!gizmoManager.Initialize(&GetMeshManager()))
    {
        MessageBox(GetWindowHandle(), L"Failed to initialize gizmo manager", L"Engine Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}


void EditorApplication::OnResize(int width, int height)
{
    UScene* scene = GetSceneManager().GetScene();
    if (scene == nullptr) return;

    UCamera* camera = scene->GetCamera();
    if (camera == nullptr) return;

    camera->SetPerspectiveDegrees(
        camera->GetFovYDegrees(),
        (height > 0) ? (float)width / (float)height : 1.0f,
        camera->GetNearZ(),
        camera->GetFarZ());
}

UScene* EditorApplication::CreateDefaultScene()
{
    return new UDefaultScene();
}
