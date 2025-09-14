#pragma once
#include "ISerializable.h"
#include "TArray.h"
#include "UMeshManager.h"
#include "USceneManager.h"
#include "json.hpp"
#include "UGizmoManager.h"

class UCamera;
class URaycastManager;

class UScene : public UObject
{
	DECLARE_UCLASS(UScene, UObject)
public:
	UScene();
	virtual ~UScene();
	virtual bool Initialize(URenderer* r, UMeshManager* mm, UInputManager* im = nullptr);

	virtual void Render();
	virtual void Update(float deltaTime);
	virtual bool OnInitialize();

	bool IsInitialized() { return bIsInitialized; }

	int32 GetObjectCount() { return PrimitiveCount; }

	static UScene* Create(json::JSON data);

	void AddObject(USceneComponent* obj);
	void SetVersion(int32 v) { Version = v; }

	json::JSON Serialize() const override;

	bool Deserialize(const json::JSON& data) override;

	const TArray<USceneComponent*>& GetObjects() const { return Objects; }
	UCamera* GetCamera() { return Camera; }
	URenderer* GetRenderer() { return Renderer; }
	UInputManager* GetInputManager() { return InputManager; }

	int32 GetBackBufferWidth() { return BackBufferWidth; };
	int32 GetBackBufferHeight() { return BackBufferHeight; };

	bool IsNameDuplicated(FName Name);

protected:
	virtual void RenderGUI() {}
	virtual void OnShutdown() {}

	int32 BackBufferWidth, BackBufferHeight;
	int32 Version;
	int32 PrimitiveCount;
	bool bIsInitialized;
	TArray<USceneComponent*> Objects;

	// Reference from outside
	URenderer* Renderer;
	UMeshManager* MeshManager;
	UInputManager* InputManager;
	//URaycastManager* RaycastManager;
	//UGizmoManager* GizmoManager;

	//UScene owns camera
	UCamera* Camera;
};