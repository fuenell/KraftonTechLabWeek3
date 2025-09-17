// UScene.cpp
#include "stdafx.h"
#include "json.hpp"
#include "UScene.h"
#include "USceneComponent.h"
#include "UPrimitiveComponent.h"
#include "UGizmoGridComp.h"
#include "URaycastManager.h"
#include "UCamera.h"
#include "ULineBatcherManager.h"
#include "USphereComp.h"
#include "UCubeComp.h"
#include "USpotLightComponent.h"

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

bool UScene::Initialize(URenderer* InRenderer, UMeshManager* InMeshManager, UInputManager* InInputManager)
{
	Renderer = InRenderer;
	MeshManager = InMeshManager;
	InputManager = InInputManager;

	BackBufferWidth = 0.0f;
	BackBufferHeight = 0.0f;

	// 모든 Primitive 컴포넌트 초기화
	for (UObject* Obj : Objects)
	{
		if (UPrimitiveComponent* Primitive = Obj->Cast<UPrimitiveComponent>())
		{
			Primitive->Init(MeshManager);
		}
	}

	Camera = new UCamera();
	Camera->SetPerspectiveDegrees(60.0f, (BackBufferHeight > 0) ? (float)BackBufferWidth / (float)BackBufferHeight : 1.0f, 0.1f, 1000.0f);
	Camera->LookAt({ -5,5,5 }, { 0,0,0 }, { 0,0,1 });

	return OnInitialize();
}

UScene* UScene::Create(json::JSON data)
{
	UScene* Scene = new UScene();
	Scene->Deserialize(data);
	return Scene;
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
	if (UPrimitiveComponent* Primitive = obj->Cast<UPrimitiveComponent>())
	{
		Primitive->Init(MeshManager);
		if (obj->CountOnInspector())
			++PrimitiveCount;
	}
}

json::JSON UScene::Serialize() const
{
	json::JSON Result;
	// UScene 특성에 맞는 JSON 구성
	Result["Version"] = Version;
	Result["NextUUID"] = std::to_string(UEngineStatics::GetNextUUID());
	int32 ValidCount = 0;
	for (UObject* Object : Objects)
	{
		if (Object == nullptr) continue;
		json::JSON JsonData = Object->Serialize();
		if (!JsonData.IsNull())
		{
			Result["Primitives"][std::to_string(Object->UUID)] = JsonData;
			++ValidCount;
		}
	}
	return Result;
}

bool UScene::Deserialize(const json::JSON& DataObject)
{
	if (!ValidateField(DataObject, "Version", json::JSON::Class::Integral) ||
		!ValidateField(DataObject, "Primitives", json::JSON::Class::Object) ||
		!ValidateField(DataObject, "NextUUID", json::JSON::Class::String))
	{
		return false;
	}

	Version = DataObject.at("Version").ToInt();

	Objects.clear();
	json::JSON PrimitivesJson = DataObject.at("Primitives");

	UEngineStatics::SetUUIDGeneration(false);
	UObject::ClearFreeIndices();

	for (auto& PrimitiveJson : PrimitivesJson.ObjectRange())
	{
		uint32 uuid = stoi(PrimitiveJson.first);
		json::JSON Data = PrimitiveJson.second;

		UClass* ClassType = nullptr;
		if (ValidateField(Data, "Type", json::JSON::Class::String))
		{
			ClassType = UClass::FindClass(Data.at("Type").ToString());

			if (ClassType == nullptr || !ClassType->IsChildOrSelfOf(USceneComponent::StaticClass()))
			{
				continue;
			}
		}
		else
		{
			continue;
		}

		USceneComponent* Component = nullptr;
		Component = ClassType->CreateDefaultObject()->Cast<USceneComponent>();

		if (Component->Deserialize(Data) == false)
		{
			delete Component;
			continue;
		}

		Component->SetUUID(uuid);

		Objects.push_back(Component);
		if (Component->CountOnInspector())
			++PrimitiveCount;
	}

	FString UuidStr = DataObject.at("NextUUID").ToString();

	UEngineStatics::SetNextUUID((uint32)stoi(UuidStr));
	UEngineStatics::SetUUIDGeneration(true);

	return true;
}

bool UScene::IsNameDuplicated(FName Name)
{
	for (UObject* Obj : Objects)
	{
		if (Obj->Name == Name)
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

	for (UObject* Obj : Objects)
	{
		if (UPrimitiveComponent* Primitive = Obj->Cast<UPrimitiveComponent>())
		{
			FBounds WorldBounds;

			// 전체 순회하는겸 미리미리 aabb계산
			FMatrix WorldMatrix = Primitive->GetWorldTransform();

			if (Primitive->GetClass() == USphereComp::StaticClass())
			{
				ULineBatcherManager::LocalSphereToWorldAABB(Primitive->GetPosition(), WorldMatrix, WorldBounds);
				Primitive->SetBoundingBox(WorldBounds);
			}
			else if (Primitive->GetClass() == UCubeComp::StaticClass() || Primitive->GetClass() == UPlaneComp::StaticClass() || Primitive->GetClass() == USpotLightComponent::StaticClass())
			{
				ULineBatcherManager::LocalAABBtoWorldAABB(Primitive->GetMesh()->GetLocalBounds(), WorldMatrix, WorldBounds);
				Primitive->SetBoundingBox(WorldBounds);
			}
			else
			{
				Primitive->UpdateAccurateBounds(WorldMatrix);	// 여기서 SetBoundingBox도 같이 됨
				WorldBounds = Primitive->GetBoundingBox();
				ULineBatcherManager::GetInstance().AddBoundingBox(WorldBounds, 0xFFFFFFFF);
			}

			Primitive->Draw(*Renderer);
		}
	}
}

void UScene::Update(float DeltaTime)
{
	Renderer->GetBackBufferSize(BackBufferWidth, BackBufferHeight);

	if (BackBufferHeight > 0)
	{
		Camera->SetAspect((float)BackBufferWidth / (float)BackBufferHeight);
	}

	float Dx = 0, Dy = 0, Dz = 0;
	bool Boost = InputManager->IsKeyDown(VK_SHIFT); // Shift로 가속

	// --- 마우스룩: RMB 누른 동안 회전 ---
	if (InputManager->IsMouseLooking())
	{
		// 마우스룩 모드는 WndProc에서 Begin/End로 관리
		float Mdx = 0.f, Mdy = 0.f;
		InputManager->ConsumeMouseDelta(Mdx, Mdy);

		float RotationSensitivity = Camera->GetRotationSensitivity(); // 일단 크게 해서 동작 확인
		Camera->AddYawPitch(Mdx * RotationSensitivity, Mdy * RotationSensitivity);
	}

	float TranslationSensitivity = Camera->GetTranslationSensitivity();

	if (InputManager->IsKeyDown('W'))
	{
		Dy += TranslationSensitivity; // 전진
	}
	if (InputManager->IsKeyDown('A'))
	{
		Dx -= TranslationSensitivity; // 좌
	}
	if (InputManager->IsKeyDown('S'))
	{
		Dy -= TranslationSensitivity; // 후진
	}
	if (InputManager->IsKeyDown('D'))
	{
		Dx += TranslationSensitivity; // 우
	}
	if (InputManager->IsKeyDown('E'))
	{
		Dz += TranslationSensitivity; // 상
	}
	if (InputManager->IsKeyDown('Q'))
	{
		Dz -= TranslationSensitivity; // 하
	}

	static float T = 0.0f; T += DeltaTime;
	// 대각선 이동 속도 보정(선택): 벡터 정규화
	Camera->MoveLocal(Dx, Dy, Dz, DeltaTime, Boost);
}

bool UScene::OnInitialize()
{
	return true;
}
