// UScene.cpp
#include "stdafx.h"
#include "json.hpp"
#include "UScene.h"
#include "UObject.h"
#include "USceneComponent.h"
#include "UPrimitiveComponent.h"
#include "UGizmoGridComp.h"
#include "URaycastManager.h"
#include "UCamera.h"

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

	backBufferWidth = 0.0f;
	backBufferHeight = 0.0f;

	camera = new UCamera();
	camera->SetPerspectiveDegrees(60.0f, (backBufferHeight > 0) ? (float)backBufferWidth / (float)backBufferHeight : 1.0f, 0.1f, 1000.0f);
	camera->LookAt({ 5,0,0 }, { 0,0,0 }, { 0,0,1 });

	// 모든 Primitive 컴포넌트 초기화
	for (UObject* obj : objects)
	{
		if (UPrimitiveComponent* primitive = dynamic_cast<UPrimitiveComponent*>(obj))
		{
			primitive->Init(meshManager);
		}
	}

	GizmoManager = new UGizmoManager(meshManager);

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

	if (!meshManager)
	{
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

json::JSON UScene::Serialize() const
{
	json::JSON result;
	// UScene 특성에 맞는 JSON 구성
	result["Version"] = version;
	result["NextUUID"] = 8;
	int validCount = 0;
	for (UObject* object : objects)
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
	// 카메라가 바뀌면 원하는 타이밍(매 프레임도 OK)에 알려주면 됨
	renderer->SetTargetAspect(camera->GetAspect());

	renderer->SetViewProj(camera->GetView(), camera->GetProj());

	for (UObject* obj : objects)
	{
		// 일단 표준 RTTI 사용
		if (UPrimitiveComponent* primitive = dynamic_cast<UPrimitiveComponent*>(obj))
		{
			primitive->Draw(*renderer);
		}
	}

	GizmoManager->Draw(*renderer);
}

void UScene::Update(float deltaTime)
{
	renderer->GetBackBufferSize(backBufferWidth, backBufferHeight);

	if (backBufferHeight > 0)
	{
		camera->SetAspect((float)backBufferWidth / (float)backBufferHeight);
	}

	float dx = 0, dy = 0, dz = 0;
	bool boost = inputManager->IsKeyDown(VK_SHIFT); // Shift로 가속

	// --- 마우스룩: RMB 누른 동안 회전 ---
	if (inputManager->IsMouseLooking())
	{
		// 마우스룩 모드는 WndProc에서 Begin/End로 관리
		float mdx = 0.f, mdy = 0.f;
		inputManager->ConsumeMouseDelta(mdx, mdy);

		const float sens = 0.005f; // 일단 크게 해서 동작 확인
		camera->AddYawPitch(-mdx * sens, -mdy * sens);
	}
	if (inputManager->IsKeyDown('W'))
	{
		dy += 1.0f; // 전진
	}
	if (inputManager->IsKeyDown('A'))
	{
		dx -= 1.0f; // 좌
	}
	if (inputManager->IsKeyDown('S'))
	{
		dy -= 1.0f; // 후진
	}
	if (inputManager->IsKeyDown('D'))
	{
		dx += 1.0f; // 우
	}
	if (inputManager->IsKeyDown('Q'))
	{
		dz += 1.0f; // 상
	}
	if (inputManager->IsKeyDown('E'))
	{
		dz -= 1.0f; // 하
	}

	static float t = 0.0f; t += deltaTime;
	// 대각선 이동 속도 보정(선택): 벡터 정규화
	float len = sqrtf(dx * dx + dy * dy + dz * dz);
	if (len > 0.f) { dx /= len; dy /= len; dz /= len; }
	camera->MoveLocal(dx, dy, dz, deltaTime, boost);

	GizmoManager->SetTarget(nullptr);
	for (UObject* obj : objects)
	{
		// 일단 표준 RTTI 사용
		if (UPrimitiveComponent* primitive = dynamic_cast<UPrimitiveComponent*>(obj))
		{
			if (primitive->bIsSelected)
			{
				GizmoManager->SetTarget(primitive);
				break;
			}
		}
	}

	// Basic rendering - nothing for now
	// 3D objects would be rendered here
		// 구 그리기
	//GetRenderer().SetViewProj(Camera.GetView(), Camera.GetProj());  // 카메라 행렬 세팅
	//sphere->SetPosition({ 0, 0.0f, 0.1f * t });
	//sphere->SetPosition({ 0, 0.0f, 0.1f * t });
	//RaycastManager->Update(GetInputManager(), *sphere);
}

bool UScene::OnInitialize()
{
	/*
	  // Manager에서 공유 Mesh 가져오기
	  UMeshManager& meshManager = GetMeshManager();
	  UMesh* sharedSphereMesh = meshManager.RetrieveMesh("Sphere");
	  UMesh* gridMesh = meshManager.RetrieveMesh("GizmoGrid");

	  // 메시가 제대로 로드되었는지 확인
	  if (!sharedSphereMesh || !gridMesh) {
		  return false; // 초기화 실패
	  }

	  RaycastManager = new URaycastManager(GetRenderer(), camera);

	  // Sphere 인스턴스 생성
	  sphere = new USphereComp(sharedSphereMesh, { 0.0f, 0.0f, 0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f });
	  sphere2 = new USphereComp(gridMesh);
	  */
	return true;
}

