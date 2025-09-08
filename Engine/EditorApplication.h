#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "USphereComp.h"
#include "URaycastManager.h"

// Simple application that inherits from UApplication
class EditorApplication : public UApplication
{
private:
	UGizmoManager gizmoManager;
	TArray<USceneComponent*> sceneComponents;
public:
	EditorApplication() = default;
	~EditorApplication() = default;
	UScene* CreateDefaultScene() override;

protected:
	void Update(float deltaTime) override;
	void Render() override;
	void RenderGUI() override;
	bool OnInitialize() override;
	void OnResize(int width, int height) override;
};