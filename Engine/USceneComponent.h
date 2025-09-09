#pragma once
#include "Matrix.h"
#include "UObject.h"
#include "Vector.h"
#include "Quaternion.h"

class USceneComponent : public UObject
{
public:
	FVector RelativeLocation;
	// FVector RelativeRotation;
	FVector RelativeScale3D;
	FQuaternion RelativeQuaternion;
	USceneComponent(FVector pos = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: RelativeLocation(pos), //RelativeRotation(rot), 
		RelativeScale3D(scl), RelativeQuaternion(FQuaternion::FromEulerXYZDeg(rot))
	{
	}

	virtual FMatrix GetWorldTransform();

	virtual std::string GetType() const
	{
		return "USceneComponent";
	}

	virtual bool IsManageable() { return false; }

	// 위치와 스케일 설정 함수들
	void SetPosition(const FVector& pos) { RelativeLocation = pos; }
	void SetScale(const FVector& scl) { RelativeScale3D = scl; }
	void SetRotation(const FVector& rot) { RelativeQuaternion = FQuaternion::FromEulerXYZDeg(rot); }
	FVector GetPosition() const { return RelativeLocation; }
	FVector GetScale() const { return RelativeScale3D; }
	FVector GetRotation() const 
	{ 
		return FQuaternion::EulerXYZDegFrom(RelativeQuaternion); 
	}

	json::JSON Serialize() const override;
	bool Deserialize(const json::JSON& data) override;
};

class USceneComponentFactory
{
	static std::unordered_map<std::string, std::function<USceneComponent* ()>>& GetCreators()
	{
		static std::unordered_map<std::string, std::function<USceneComponent* ()>> creators;
		return creators;
	}
public:
	static void Register(const std::string& typeName, std::function<USceneComponent* ()> creator)
	{
		GetCreators()[typeName] = creator;
	}

	static USceneComponent* Create(const std::string& typeName)
	{
		auto& creators = GetCreators();
		auto it = creators.find(typeName);
		return (it != creators.end()) ? it->second() : nullptr;
	}

	// 등록된 타입 목록 조회 (디버깅용)
	static std::vector<std::string> GetRegisteredTypes()
	{
		std::vector<std::string> types;
		for (const auto& pair : GetCreators())
		{
			types.push_back(pair.first);
		}
		return types;
	}
};
