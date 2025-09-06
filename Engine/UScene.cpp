// UScene.cpp
#include "stdafx.h"
#include "UScene.h"
#include "UObject.h"

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
