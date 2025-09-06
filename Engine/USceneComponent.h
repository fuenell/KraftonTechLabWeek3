#pragma once
#include "UObject.h"
#include "Vector.h"

class USceneComponent : public UObject
{

public:
    FVector RelativeLocation;
    FVector RelativeRotation;
    FVector RelativeScale3D;

    virtual std::string GetType() const
    {
        return "USceneComponent";
    }

    json::JSON Serialize() const override;
    bool Deserialize(const json::JSON& data) override;


};

class USceneComponentFactory {
    static std::unordered_map<std::string, std::function<USceneComponent* (json::JSON)>>& GetCreators() {
        static std::unordered_map<std::string, std::function<USceneComponent* (json::JSON)>> creators;
        return creators;
    }
public:
    static void Register(const std::string& typeName, std::function<USceneComponent* (json::JSON)> creator) {
        GetCreators()[typeName] = creator;
    }

    static USceneComponent* Create(const std::string& typeName, json::JSON data) {
        auto& creators = GetCreators();
        auto it = creators.find(typeName);
        return (it != creators.end()) ? it->second(data) : nullptr;
    }

    // 등록된 타입 목록 조회 (디버깅용)
    static std::vector<std::string> GetRegisteredTypes() {
        std::vector<std::string> types;
        for (const auto& pair : GetCreators()) {
            types.push_back(pair.first);
        }
        return types;
    }
};
