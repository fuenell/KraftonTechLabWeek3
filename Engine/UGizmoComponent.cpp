#include "stdafx.h"
#include "UGizmoComponent.h"
#include "UMeshManager.h"

bool UGizmoComponent::Init(UMeshManager* meshManager)
{
	if (meshManager)
	{
		mesh = meshManager->RetrieveMesh(GetType());
		return mesh != nullptr;
	}
	return false;
}
