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

    if (GetInputManager().IsKeyDown(VK_ESCAPE))
    {
        RequestExit();
    }

    UGizmoComponent* hitGizmo = nullptr;
    UPrimitiveComponent* hitPrimitive = nullptr;

    if (GetInputManager().IsMouseButtonReleased(0))
    {
        bIsMouseButtonDown = false;
        return;   
    }
    
    if (bIsMouseButtonDown)
    {
        // 드래그 하고 있을때

        return;
    }
    
    if (GetInputManager().IsMouseButtonPressed(0))
    {
        bIsMouseButtonDown = true;

        TArray<UPrimitiveComponent*> primitives;
        TArray<UGizmoComponent*> gizmos;

        TArray<UGizmoComponent*>& g = gizmoManager.GetRaycastableGizmos();
        if (g.size() > 0)
        {
            for (UGizmoComponent* gizmo : g)
            {
                gizmos.push_back(gizmo);
                gizmo->bIsSelected = false;
            }   
        }

        for (UObject* obj : GetSceneManager().GetScene()->GetObjects())
        {
            if (UPrimitiveComponent* primitive = dynamic_cast<UPrimitiveComponent*>(obj))
            {
                if (primitive->GetMesh()) primitives.push_back(primitive);
                primitive->bIsSelected = false;
            }
        }

        if (GetRaycastManager().RayIntersectsMeshes(GetSceneManager().GetScene()->GetCamera(), gizmos, hitGizmo))
        {
            if (auto target = gizmoManager.GetTarget())
            {
                target->bIsSelected = true;
                hitGizmo->bIsSelected = true;
                if (target->IsManageable())
                    propertyWindow->SetTarget(target);
            }
        }
        else if (GetRaycastManager().RayIntersectsMeshes(GetSceneManager().GetScene()->GetCamera(), primitives, hitPrimitive))
        {
            gizmoManager.SetTarget(hitPrimitive);
            hitPrimitive->bIsSelected = true;
            if (hitPrimitive->IsManageable())
                propertyWindow->SetTarget(hitPrimitive);
        }
        else
        {
            gizmoManager.SetTarget(nullptr);
            propertyWindow->SetTarget(nullptr);
        }
    }
}

void EditorApplication::Render()
{
    gizmoManager.Draw(GetRenderer());
    UApplication::Render();
}

void EditorApplication::RenderGUI()
{
    controlPanel->Render();
    propertyWindow->Render();

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

    controlPanel = new UControlPanel(&GetSceneManager());
    propertyWindow = new USceneComponentPropertyWindow();

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
