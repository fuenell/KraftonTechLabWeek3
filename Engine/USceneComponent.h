#pragma once
#include "Matrix.h"
#include "UObject.h"
#include "Vector.h"

class USceneComponent : public UObject
{
public:
	FVector RelativeLocation;
	FVector RelativeRotation;
	FVector RelativeScale3D;

	USceneComponent(FVector pos = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: RelativeLocation(pos), RelativeRotation(rot), RelativeScale3D(scl)
	{
	}
    FMatrix GetWorldTransform();

    virtual std::string GetType() const
    {
        return "USceneComponent";
    }

	// 위치와 스케일 설정 함수들
	void SetPosition(const FVector& pos) { RelativeLocation = pos; }
	void SetScale(const FVector& scl) { RelativeScale3D = scl; }
	void SetRotation(const FVector& rot) { RelativeRotation = rot; }

	FVector GetPosition() const { return RelativeLocation; }
	FVector GetScale() const { return RelativeScale3D; }
	FVector GetRotation() const { return RelativeRotation; }

	json::JSON Serialize() const override;
	bool Deserialize(const json::JSON& data) override;
};

class USceneComponentFactory
{
	static std::unordered_map<std::string, std::function<USceneComponent* (json::JSON)>>& GetCreators()
	{
		static std::unordered_map<std::string, std::function<USceneComponent* (json::JSON)>> creators;
		return creators;
	}
public:
	static void Register(const std::string& typeName, std::function<USceneComponent* (json::JSON)> creator)
	{
		GetCreators()[typeName] = creator;
	}

	static USceneComponent* Create(const std::string& typeName, json::JSON data)
	{
		auto& creators = GetCreators();
		auto it = creators.find(typeName);
		return (it != creators.end()) ? it->second(data) : nullptr;
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
