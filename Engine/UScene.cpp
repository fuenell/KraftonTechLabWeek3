// UScene.cpp
#include "stdafx.h"
#include "json.hpp"
#include "UScene.h"
#include "USceneComponent.h"
#include "UPrimitiveComponent.h"
#include "UGizmoGridComp.h"
#include "URaycastManager.h"
#include "UCamera.h"

IMPLEMENT_UCLASS(UScene, UObject)
UScene::UScene()
{
	Version = 1;
	PrimitiveCount = 0;
}

UScene::~UScene()
{
	OnShutdown();
	for (UObject* Object : Objects)
	{
		delete Object;
	}
	delete Camera;
}

bool UScene::Initialize(URenderer* r, UMeshManager* mm, UInputManager* im)
{
	Renderer = r;
	MeshManager = mm;
	InputManager = im;

	BackBufferWidth = 0.0f;
	BackBufferHeight = 0.0f;

	// 모든 Primitive 컴포넌트 초기화
	for (UObject* obj : Objects)
	{
		if (UPrimitiveComponent* primitive = obj->Cast<UPrimitiveComponent>())
		{
			primitive->Init(MeshManager);
		}
	}

	Camera = new UCamera();
	Camera->SetPerspectiveDegrees(60.0f, (BackBufferHeight > 0) ? (float)BackBufferWidth / (float)BackBufferHeight : 1.0f, 0.1f, 1000.0f);
	Camera->LookAt({ -5,5,5 }, { 0,0,0 }, { 0,0,1 });

	return OnInitialize();
}

UScene* UScene::Create(json::JSON data)
{
	UScene* scene = new UScene();
	scene->Deserialize(data);
	return scene;
}

void UScene::AddObject(USceneComponent* obj)
{
	// 런타임에서만 사용 - Scene이 Initialize된 후에 호출할 것
	assert(MeshManager != nullptr && "AddObject should only be called after Scene initialization");

	if (!MeshManager)
	{
		// 릴리즈 빌드에서 안전성 확보
		return;
	}

	Objects.push_back(obj);

	// 일단 표준 RTTI 사용
	if (UPrimitiveComponent* primitive = obj->Cast<UPrimitiveComponent>())
	{
		primitive->Init(MeshManager);
		if (obj->CountOnInspector())
			++PrimitiveCount;
	}
}

json::JSON UScene::Serialize() 

const
{
	json::JSON result;
	// UScene 특성에 맞는 JSON 구성
	result["Version"] = Version;
	result["NextUUID"] = std::to_string(UEngineStatics::GetNextUUID());
	int32 validCount = 0;
	for (UObject* object : Objects)
	{
		if (object == nullptr) continue;
		json::JSON _json = object->Serialize();
		if (!_json.IsNull())
		{
			//result["Primitives"][std::to_string(validCount)] = _json;
			result["Primitives"][std::to_string(object->UUID)] = _json;
			++validCount;
		}
	}
	return result;
}

bool UScene::Deserialize(const json::JSON& data)
{
	Version = data.at("Version").ToInt();
	// nextUUID = data.at("NextUUID").ToInt();

	Objects.clear();
	json::JSON primitivesJson = data.at("Primitives");

	UEngineStatics::SetUUIDGeneration(false);
	UObject::ClearFreeIndices();
	for (auto& primitiveJson : primitivesJson.ObjectRange())
	{
		uint32 uuid = stoi(primitiveJson.first);
		json::JSON _data = primitiveJson.second;

		UClass* _class = UClass::FindClassWithTypeName(_data.at("Type").ToString());
		USceneComponent* component = nullptr;
		if (_class != nullptr) component = _class->CreateDefaultObject()->Cast<USceneComponent>();

		component->Deserialize(_data);
		component->SetUUID(uuid);

		Objects.push_back(component);
		if (component->CountOnInspector())
			++PrimitiveCount;
	}

	FString uuidStr = data.at("NextUUID").ToString();

	UEngineStatics::SetNextUUID((uint32)stoi(uuidStr));
	UEngineStatics::SetUUIDGeneration(true);

	return true;
}

bool UScene::IsNameDuplicated(FName Name)
{
	for (UObject* obj : Objects)
	{
		if (obj->Name == Name)
		{
			return true;
		}
	}

	return false;
}

void UScene::Render()
{
	// 카메라가 바뀌면 원하는 타이밍(매 프레임도 OK)에 알려주면 됨
	Renderer->SetTargetAspect(Camera->GetAspect());

	Renderer->SetViewProj(Camera->GetView(), Camera->GetProj());

	for (UObject* obj : Objects)
	{
		if (UPrimitiveComponent* primitive = obj->Cast<UPrimitiveComponent>())
		{
			primitive->Draw(*Renderer);
		}
	}
}

void UScene::Update(float deltaTime)
{
	Renderer->GetBackBufferSize(BackBufferWidth, BackBufferHeight);

	if (BackBufferHeight > 0)
	{
		Camera->SetAspect((float)BackBufferWidth / (float)BackBufferHeight);
	}

	float dx = 0, dy = 0, dz = 0;
	bool boost = InputManager->IsKeyDown(VK_SHIFT); // Shift로 가속

	// --- 마우스룩: RMB 누른 동안 회전 ---
	if (InputManager->IsMouseLooking())
	{
		// 마우스룩 모드는 WndProc에서 Begin/End로 관리
		float mdx = 0.f, mdy = 0.f;
		InputManager->ConsumeMouseDelta(mdx, mdy);

		float RotationSensitivity = Camera->GetRotationSensitivity(); // 일단 크게 해서 동작 확인
		Camera->AddYawPitch(mdx * RotationSensitivity, mdy * RotationSensitivity);
	}

	float TranslationSensitivity = Camera->GetTranslationSensitivity();

	if (InputManager->IsKeyDown('W'))
	{
		dy += TranslationSensitivity; // 전진
	}
	if (InputManager->IsKeyDown('A'))
	{
		dx -= TranslationSensitivity; // 좌
	}
	if (InputManager->IsKeyDown('S'))
	{
		dy -= TranslationSensitivity; // 후진
	}
	if (InputManager->IsKeyDown('D'))
	{
		dx += TranslationSensitivity; // 우
	}
	if (InputManager->IsKeyDown('E'))
	{
		dz += TranslationSensitivity; // 상
	}
	if (InputManager->IsKeyDown('Q'))
	{
		dz -= TranslationSensitivity; // 하
	}

	static float t = 0.0f; t += deltaTime;
	// 대각선 이동 속도 보정(선택): 벡터 정규화
	Camera->MoveLocal(dx, dy, dz, deltaTime, boost);
}

bool UScene::OnInitialize()
{

	return true;
}

