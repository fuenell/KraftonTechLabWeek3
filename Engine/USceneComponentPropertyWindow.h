#pragma once
#include "ImGuiWindowWrapper.h"
#include "USceneManager.h"
#include "USceneComponent.h"

class USceneComponentPropertyWindow : public ImGuiWindowWrapper
{
private:
	USceneComponent* Target = nullptr;

public:
	USceneComponentPropertyWindow() : ImGuiWindowWrapper("Jungle Property Window")
	{

	}
	void SetTarget(USceneComponent* target) 
	{
		Target = target;
	}
	void RenderContent() override;
};

