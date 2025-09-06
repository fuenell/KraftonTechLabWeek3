#pragma once
#include "ISerializable.h"
#include "Globals.h"

namespace json { class JSON; } // forward declaration

class UScene : ISerializable
{
    int version;
    TArray<UObject*> objects;
public:
    json::JSON Serialize() const override;
    bool Deserialize(const json::JSON& data) override;
    void AddObject(UObject* obj)
    {
        objects.push_back(obj);
    }
};

