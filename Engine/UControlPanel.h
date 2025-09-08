#pragma once
#include "ImGuiWindowWrapper.h"
#include "USceneManager.h"

class UControlPanel : public ImGuiWindowWrapper
{
	USceneManager* SceneManager;
	
	// Spawn Primitive Section
	int primitiveChoiceIndex = 0;

	// Scene Management Section
	char sceneName[256] = "";

	// Camera Management Section


public:
	UControlPanel(USceneManager* sceneManager) : ImGuiWindowWrapper("Jungle Control Panel")
	{
		SceneManager = sceneManager;
	}
	void RenderContent() override;
	void PrimaryInformationSection();
	void SpawnPrimitiveSection();
	void SceneManagementSection();
	void CameraManagementSection();
};

