// UScene.cpp
#include "stdafx.h"
#include "json.hpp"
#include "UScene.h"
#include "UObject.h"
#include "USceneComponent.h"
#include "UPrimitiveComponent.h"
#include "UGizmoGridComp.h"

UScene::UScene()
{
    version = 1;

    primitiveCount = 0;
}

UScene::~UScene()
{
    OnShutdown();
    for (UObject* object : objects)
    {
        delete object;
    }
}

bool UScene::Initialize(URenderer* r, UMeshManager* mm, UInputManager* im)
{
    renderer = r;
    meshManager = mm;
    inputManager = im;

    // 모든 Primitive 컴포넌트 초기화
    for (UObject* obj : objects) {
        if (UPrimitiveComponent* primitive = dynamic_cast<UPrimitiveComponent*>(obj)) {
            primitive->Init(meshManager);
        }
    }

    return true;
}

UScene* UScene::Create(json::JSON data)
{
    UScene* scene = new UScene();
    scene->Deserialize(data);
    return scene;
}

void UScene::AddObject(UObject* obj)
{
    // 런타임에서만 사용 - Scene이 Initialize된 후에 호출할 것
    assert(meshManager != nullptr && "AddObject should only be called after Scene initialization");
    
    if (!meshManager) {
        // 릴리즈 빌드에서 안전성 확보
        return;
    }
    
    objects.push_back(obj);

    // 일단 표준 RTTI 사용
    if (UPrimitiveComponent* primitive = dynamic_cast<UPrimitiveComponent*>(obj))
    {
        primitive->Init(meshManager);
        ++primitiveCount;
    }
}

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

        //AddObject(component);
        objects.push_back(component);
    }
    
    USceneComponent* gizmoGrid = new UGizmoGridComp(
        { 0.3f, 0.3f, 0.3f },
        { 0.0f, 0.0f, 0.0f },
        { 0.2f, 0.2f, 0.2f }
    );
    objects.push_back(gizmoGrid);

    return true;
}

void UScene::Render()
{
    for (UObject* obj : objects)
    {
        // 일단 표준 RTTI 사용
        if (UPrimitiveComponent* primitive = dynamic_cast<UPrimitiveComponent*>(obj))
        {
            primitive->Draw(*renderer);
        }
    }
}

