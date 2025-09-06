#include "stdafx.h"
#include "UDefaultScene.h"

void UDefaultScene::Update(float deltaTime)
{
    static float t = 0.0f;
    t += deltaTime;
    sphere->SetPosition({ 0, 0.0f, 0.1f * t });
}

bool UDefaultScene::OnInitialize()
{
    // 컴포넌트 생성
    sphere = new USphereComp({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f });
    gizmoGrid = new UGizmoGridComp({ 0.3f, 0.3f, 0.3f }, { 0.0f, 0.0f, 0.0f }, { 0.2f, 0.2f, 0.2f });

    AddObject(sphere);
    AddObject(gizmoGrid);

	return true;
}
