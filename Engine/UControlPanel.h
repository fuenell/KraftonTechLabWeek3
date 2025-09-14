#pragma once
#include "ImGuiWindowWrapper.h"
#include "USceneManager.h"
#include "USceneComponent.h"
#include "UGizmoManager.h"
#include "ULineBatcherManager.h"
#include "URenderer.h"

class UControlPanel : public ImGuiWindowWrapper
{
private:
	USceneManager* SceneManager;
	UGizmoManager* GizmoManager;
	ULineBatcherManager* LineBatcherManager;
	URenderer* Renderer;

	// Spawn Primitive Section
	TArray<UClass*> registeredTypes;
	TArray<FString> choiceStrList;
	TArray<const char*> choices;
	int32 primitiveChoiceIndex = 0;

	// Scene Management Section
	char sceneName[256] = "Default";

	// Camera Management Section


public:
	UControlPanel(USceneManager* sceneManager, UGizmoManager* gizmoManager, ULineBatcherManager* LineBatcherManager, URenderer* Renderer);
	void RenderContent() override;
	void PrimaryInformationSection();
	void SpawnPrimitiveSection();
	void SceneManagementSection();
	void CameraManagementSection();
	void GridManagementSection();
	void ViewModeSection();
	USceneComponent* CreateSceneComponentFromChoice(int index);
};

