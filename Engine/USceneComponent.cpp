// USceneComponent.cpp
#include "stdafx.h"
#include "USceneComponent.h"
#include "json.hpp"
#include "UScene.h"

IMPLEMENT_UCLASS(USceneComponent, UObject)
FMatrix USceneComponent::GetWorldTransform()
{
	return FMatrix::SRTRowQuaternion(RelativeLocation, RelativeQuaternion.ToMatrixRow(), RelativeScale3D);
}

json::JSON USceneComponent::Serialize() const
{
	FVector TmpRot = RelativeQuaternion.GetEulerXYZ();
	json::JSON Result;
	Result["Type"] = GetClass()->GetUClassName();
	Result["FName"] = Name;
	Result["Location"] = json::Array(RelativeLocation.X, RelativeLocation.Y, RelativeLocation.Z);
	Result["Rotation"] = json::Array(TmpRot.X, TmpRot.Y, TmpRot.Z);
	Result["Scale"] = json::Array(RelativeScale3D.X, RelativeScale3D.Y, RelativeScale3D.Z);
	return Result;
}

// -------------------------
// Deserialize
// -------------------------
bool USceneComponent::Deserialize(const json::JSON& DataObject)
{
	// "FName" 키가 있는지 확인 + string 타입인지 검사
	if (UScene::ValidateField(DataObject, "FName", json::JSON::Class::String))
	{
		FString NameString = DataObject.at("FName").ToString();
		this->Name = FName(NameString);
	}

	if (UScene::ValidateField(DataObject, "Location", json::JSON::Class::Array))
	{
		json::JSON Loc = DataObject.at("Location");
		if (Loc.size() != 3) return false;
		RelativeLocation = FVector(Loc[0].ToFloat(), Loc[1].ToFloat(), Loc[2].ToFloat());
	}

	if (UScene::ValidateField(DataObject, "Rotation", json::JSON::Class::Array))
	{
		json::JSON Rot = DataObject.at("Rotation");
		if (Rot.size() != 3) return false;
		RelativeQuaternion = FQuaternion::FromEulerXYZ(Rot[0].ToFloat(), Rot[1].ToFloat(), Rot[2].ToFloat());
	}

	if (UScene::ValidateField(DataObject, "Scale", json::JSON::Class::Array))
	{
		json::JSON Scale = DataObject.at("Scale");
		if (Scale.size() != 3) return false;
		RelativeScale3D = FVector(Scale[0].ToFloat(), Scale[1].ToFloat(), Scale[2].ToFloat());
	}
}