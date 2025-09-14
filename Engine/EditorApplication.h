#pragma once
#include "stdafx.h"
#include "Application.h"
#include "USphereComp.h"
#include "URaycastManager.h"
#include "UControlPanel.h"
#include "USceneComponentPropertyWindow.h"
#include "USceneManagerWindow.h"
#include "ToggleWindow.h"

class USceneManagerWindow;

// Simple application that inherits from UApplication
class EditorApplication : public Application
{
public:
	EditorApplication() = default;
	~EditorApplication()
	{
		delete controlPanel;
		delete propertyWindow;
		delete SceneManagerWindow;
		delete ToggleWindow;
		controlPanel = nullptr;
		propertyWindow = nullptr;
		SceneManagerWindow = nullptr;
		ToggleWindow = nullptr;
	}
	UScene* CreateDefaultScene() override;

	void OnSceneChange() override;
	void SetTarget(UPrimitiveComponent* Target);

protected:
	void Update(float deltaTime) override;
	void Render() override;
	void RenderGUI() override;
	bool OnInitialize() override;
	void OnResize(int32 width, int32 height) override;

private:
	void ProcessKeyboardInput();
	void ProcessMouseInteraction();

private:
	UGizmoManager gizmoManager;
	TArray<USceneComponent*> sceneComponents;

	USceneComponent* selectedSceneComponent;

	UControlPanel* controlPanel;
	USceneComponentPropertyWindow* propertyWindow;
	USceneManagerWindow* SceneManagerWindow;
	UToggleWindow* ToggleWindow;
};