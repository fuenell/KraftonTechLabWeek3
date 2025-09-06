// UScene.cpp
#include "stdafx.h"
#include "json.hpp"
#include "UScene.h"
#include "UObject.h"
#include "USceneComponent.h"

json::JSON UScene::Serialize() const {
    json::JSON result;
    // UScene 특성에 맞는 JSON 구성
    result["Version"] = version;
    result["NextUUID"] = 8;
    int validCount = 0;
    for(UObject* object : objects)
    {
        json::JSON _json = object->Serialize();
        if (!_json.IsNull())
        {
            result["Primitives"][std::to_string(validCount)] = _json;
            ++validCount;
        }
    }
    return result;
}

bool UScene::Deserialize(const json::JSON& data)
{
    version = data.at("Version").ToInt();
    // nextUUID = data.at("NextUUID").ToInt();

    objects.clear();
    json::JSON primitivesJson = data.at("Primitives");

    for (auto& primitiveJson : primitivesJson.ObjectRange())
    {
        
        int index = stoi(primitiveJson.first);
        json::JSON _data = primitiveJson.second;

        USceneComponent* component = USceneComponentFactory::Create(_data.at("Type").ToString(), _data);

        objects.push_back(component);
    }

    return true;
}
