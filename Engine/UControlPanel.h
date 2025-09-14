#pragma once
#include "ImGuiWindowWrapper.h"
#include "USceneManager.h"
#include "USceneComponent.h"
#include "UGizmoManager.h"
#include "ULineBatcherManager.h"
#include "URenderer.h"

class UControlPanel : public ImGuiWindowWrapper
{
public:
	UControlPanel(USceneManager* InSceneManager, UGizmoManager* InGizmoManager, ULineBatcherManager* InLineBatcherManager, URenderer* InRenderer);
	void RenderContent() override;
	void PrimaryInformationSection();
	void SpawnPrimitiveSection();
	void SceneManagementSection();
	void CameraManagementSection();
	void GridManagementSection();
	void ViewModeSection();
	USceneComponent* CreateSceneComponentFromChoice(int Index);

private:
	USceneManager* SceneManager;
	UGizmoManager* GizmoManager;
	ULineBatcherManager* LineBatcherManager;
	URenderer* Renderer;

	// Spawn Primitive Section
	TArray<UClass*> RegisteredTypes;
	TArray<FString> ChoiceStrList;
	TArray<const char*> Choices;
	int32 PrimitiveChoiceIndex = 0;
};

