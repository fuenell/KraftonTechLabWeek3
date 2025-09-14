#include "stdafx.h"
#include "UDefaultScene.h"
#include "URaycastManager.h"

void UDefaultScene::Update(float DeltaTime)
{
	UScene::Update(DeltaTime);
	static float T = 0.0f;
	T += DeltaTime;
}

bool UDefaultScene::OnInitialize()
{
	UScene::OnInitialize();
	if (IsFirstTime)
	{
		IsFirstTime = false;
	}

	return true;
}
