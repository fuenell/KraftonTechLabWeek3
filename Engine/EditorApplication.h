#pragma once
#include "stdafx.h"
#include "Application.h"
#include "USphereComp.h"
#include "URaycastManager.h"
#include "UControlPanel.h"
#include "USceneComponentPropertyWindow.h"
#include "USceneManagerWindow.h"
#include "UToggleWindow.h"

class USceneManagerWindow;

// Simple application that inherits from UApplication
class EditorApplication : public Application
{
public:
	EditorApplication() {};
	~EditorApplication()
	{
		delete ControlPanel;
		delete PropertyWindow;
		delete SceneManagerWindow;
		delete ToggleWindow;
		ControlPanel = nullptr;
		PropertyWindow = nullptr;
		SceneManagerWindow = nullptr;
		ToggleWindow = nullptr;
	}

	void OnSceneChange() override;
	void SetTarget(UPrimitiveComponent* Target);

protected:
	void Update(float DeltaTime) override;
	void Render() override;
	void RenderGUI() override;
	bool OnInitialize() override;
	void OnResize(int32 Width, int32 Height) override;

private:
	void ProcessKeyboardInput();
	void ProcessMouseInteraction();

private:
	UGizmoManager GizmoManager;

	UControlPanel* ControlPanel{};
	USceneComponentPropertyWindow* PropertyWindow{};
	USceneManagerWindow* SceneManagerWindow{};
	UToggleWindow* ToggleWindow{};
};