#include "stdafx.h"
#include "UPrimitiveComponent.h"
#include "UMeshManager.h"

bool UPrimitiveComponent::Init(UMeshManager* meshManager) {
	if (meshManager) {
		mesh = meshManager->RetrieveMesh(GetType());
		return mesh != nullptr;
	}
	return false;
}
