#pragma once
#include "ImGuiWindowWrapper.h"
#include "USceneComponent.h"
#include "USceneManager.h"
#include "UPrimitiveComponent.h"

using FOnComponentSelected = std::function<void(UPrimitiveComponent*)>;

class USceneManagerWindow : public ImGuiWindowWrapper
{
public:
	USceneManagerWindow(USceneManager* InSceneManager, FOnComponentSelected InOnComponentSelected) :
		SceneManager(InSceneManager),
		OnComponentSelected(InOnComponentSelected),
		ImGuiWindowWrapper("Scene Manager", ImVec2(275, 0), ImVec2(275, 250))
	{

	}

	~USceneManagerWindow()
	{
		SceneManager = nullptr;
		Target = nullptr;
	}

	void SetTarget(USceneComponent* target)
	{
		Target = target;
	}

	void RenderContent() override;

private:
	FOnComponentSelected OnComponentSelected;
	USceneManager* SceneManager = nullptr;
	USceneComponent* Target = nullptr;
};
