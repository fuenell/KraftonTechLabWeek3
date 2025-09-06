#pragma once
#include "ISerializable.h"
#include "Globals.h"
#include "UMeshManager.h"
#include "USceneManager.h"
#include "json.hpp"
#include "UGizmoManager.h"

class UCamera;
class URaycastManager;

class UScene : ISerializable
{
protected:
    int backBufferWidth, backBufferHeight;
    int version;
    int primitiveCount;
    bool isInitialized;
    TArray<UObject*> objects;

    // Reference from outside
    URenderer* renderer;
    UMeshManager* meshManager;
    UInputManager* inputManager;

    //UScene owns camera
    UCamera* camera;
    URaycastManager* RaycastManager;
    UGizmoManager* GizmoManager;

    virtual void RenderGUI() {}
    virtual void OnShutdown() {}
public:
    UScene();
    virtual ~UScene();
    virtual bool Initialize(URenderer* r, UMeshManager* mm, UInputManager* im = nullptr);

    virtual void Render();
    virtual void Update(float deltaTime);
    virtual bool OnInitialize();

    bool IsInitialized() { return isInitialized; }

    int GetObjectCount() { return primitiveCount; }

    static UScene* Create(json::JSON data);

    void AddObject(UObject* obj);

    json::JSON Serialize() const override;

    bool Deserialize(const json::JSON& data) override;

    UCamera* GetCamera() { return camera; }
    URenderer* GetRenderer() { return renderer; }
    UInputManager* GetInputManager() { return inputManager; }
};