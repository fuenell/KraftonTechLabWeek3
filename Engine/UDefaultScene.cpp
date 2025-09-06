#include "stdafx.h"
#include "UDefaultScene.h"

void UDefaultScene::Update(float deltaTime)
{
    UScene::Update(deltaTime);
    static float t = 0.0f;
    t += deltaTime;
}

bool UDefaultScene::OnInitialize()
{
    UScene::OnInitialize();
    // 컴포넌트 생성
    USphereComp* sphere = new USphereComp({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f });
    UGizmoGridComp* gizmoGrid = new UGizmoGridComp({ 0.3f, 0.3f, 0.3f }, { 0.0f, 0.0f, 0.0f }, { 0.2f, 0.2f, 0.2f });

    AddObject(sphere);
    AddObject(gizmoGrid);

	return true;
}
