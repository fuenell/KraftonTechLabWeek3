#pragma once
#include "ISerializable.h"
#include "Globals.h"
#include "UMeshManager.h"
#include "USceneManager.h"
#include "json.hpp"

class UScene : ISerializable
{
protected:
    int version;
    int primitiveCount;
    bool isInitialized;
    TArray<UObject*> objects;

    URenderer* renderer;
    UMeshManager* meshManager;
    UInputManager* inputManager;

    virtual void RenderGUI() {}
    virtual void OnShutdown() {}
public:
    UScene();
    virtual ~UScene();
    virtual bool Initialize(URenderer* r, UMeshManager* mm, UInputManager* im = nullptr);

    virtual void Render();
    virtual void Update(float deltaTime) {}
    virtual bool OnInitialize() {
        return true;
    }

    bool IsInitialized() { return isInitialized; }

    int GetObjectCount() { return primitiveCount; }

    static UScene* Create(json::JSON data);

    void AddObject(UObject* obj);

    json::JSON Serialize() const override;

    bool Deserialize(const json::JSON& data) override;
};