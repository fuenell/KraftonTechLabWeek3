#include "stdafx.h"
#include "Application.h"
#include "EditorApplication.h"
#include "UMeshManager.h"
#include "ImguiConsole.h"
#include "UScene.h"
#include "UDefaultScene.h"
#include "URaycastManager.h"
#include "UGizmoArrowComp.h"
#include "UGizmoRotationHandleComp.h"
#include "UGizmoScaleHandleComp.h"
#include "USphereComp.h"
#include "UCubeComp.h"
#include "UPlaneComp.h"
#include "USpotLightComponent.h"

void EditorApplication::Update(float deltaTime)
{
	GetSceneManager().GetScene()->Update(deltaTime);

	gizmoManager.Update(deltaTime);

	ProcessKeyboardInput();

	ProcessMouseInteraction();
}

void EditorApplication::ProcessKeyboardInput()
{
	if (GetInputManager().IsKeyDown(VK_ESCAPE))
	{
		RequestExit();
	}
	if (GetInputManager().IsKeyPressed(VK_SPACE))
	{
		gizmoManager.NextTranslation();
	}
	if (GetInputManager().IsKeyPressed('X'))
	{
		gizmoManager.ChangeGizmoSpace();
	}
}

void EditorApplication::ProcessMouseInteraction()
{
	// =================================================================
	// 상태 1: 기즈모를 드래그하고 있는 중일 때의 로직
	// =================================================================
	if (gizmoManager.IsDragging())
	{
		// 마우스 왼쪽 버튼을 떼면 드래그를 종료합니다.
		if (GetInputManager().IsMouseButtonReleased(0))
		{
			gizmoManager.EndDrag();
		}
		// 드래그가 계속되고 있다면 마우스 위치에 따라 기즈모를 업데이트합니다.
		else
		{
			FRay ray = GetRaycastManager().CreateRayFromScreenPosition(GetSceneManager().GetScene()->GetCamera());
			gizmoManager.UpdateDrag(ray);
		}
	}
	// =================================================================
	// 상태 2: 드래그 중이 아닐 때 (새로운 상호작용을 시작할지 확인)
	// =================================================================
	else
	{
		// ImGui UI가 마우스 입력을 사용하고 있다면, 씬에 대한 상호작용은 무시합니다.
		if (ImGui::GetIO().WantCaptureMouse)
		{
			return;
		}

		// 마우스 왼쪽 버튼을 눌렀을 때만 피킹(Picking)을 시도합니다.
		if (GetInputManager().IsMouseButtonPressed(0))
		{
			FVector outImpactPoint;
			UGizmoComponent* hitGizmo = nullptr;
			UPrimitiveComponent* hitPrimitive = nullptr;

			// 씬의 모든 PrimitiveComponent와 GizmoComponent를 수집합니다.
			TArray<UPrimitiveComponent*> primitives;
			for (UObject* obj : GetSceneManager().GetScene()->GetObjects())
			{
				if (UPrimitiveComponent* primitive = obj->Cast<UPrimitiveComponent>())
				{
					if (primitive->GetMesh()) primitives.push_back(primitive);
					primitive->bIsSelected = false;
				}
			}

			TArray<UGizmoComponent*> gizmos;
			TArray<UGizmoComponent*>& g = gizmoManager.GetRaycastableGizmos();
			if (g.size() > 0)
			{
				for (UGizmoComponent* gizmo : g)
				{
					gizmos.push_back(gizmo);
					gizmo->bIsSelected = false;
				}
			}

			// 1. 기즈모에 대한 피킹을 먼저 시도합니다.
			if (GetRaycastManager().RayIntersectsMeshes(GetSceneManager().GetScene()->GetCamera(), gizmos, hitGizmo, outImpactPoint))
			{
				hitGizmo->bIsSelected = true;
				FRay ray = GetRaycastManager().CreateRayFromScreenPosition(GetSceneManager().GetScene()->GetCamera());

				// 히트된 기즈모의 종류에 따라 적절한 드래그를 시작합니다.
				if (UGizmoArrowComp* arrow = hitGizmo->Cast<UGizmoArrowComp>())
				{
					gizmoManager.BeginDrag(ray, arrow->Axis, outImpactPoint, GetSceneManager().GetScene());
				}
				else if (UGizmoRotationHandleComp* rotationHandle = hitGizmo->Cast<UGizmoRotationHandleComp>())
				{
					gizmoManager.BeginDrag(ray, rotationHandle->Axis, outImpactPoint, GetSceneManager().GetScene());
				}
				else if (UGizmoScaleHandleComp* scaleHandle = hitGizmo->Cast<UGizmoScaleHandleComp>())
				{
					gizmoManager.BeginDrag(ray, scaleHandle->Axis, outImpactPoint, GetSceneManager().GetScene());
				}
			}
			// 2. 기즈모가 아니라면, 일반 오브젝트에 대한 피킹을 시도합니다.
			else if (GetRaycastManager().RayIntersectsMeshes(GetSceneManager().GetScene()->GetCamera(), primitives, hitPrimitive, outImpactPoint))
			{
				hitPrimitive->bIsSelected = true;

				if (hitPrimitive->IsManageable())
				{
					SetTarget(hitPrimitive);
				}
			}
			// 3. 아무것도 피킹되지 않았다면 모든 선택을 해제합니다.
			else
			{
				SetTarget(nullptr);
			}
		}
	}
}

void EditorApplication::Render()
{
	ID3D11Device* Device = renderer.GetDevice();
	ID3D11DeviceContext* DeviceContext = renderer.GetDeviceContext();

	const FMatrix View = sceneManager.GetScene()->GetCamera()->GetView();   // 네 쪽의 뷰 행렬 getter
	const FMatrix Proj = sceneManager.GetScene()->GetCamera()->GetProj();   // 네 쪽의 프로젝션 행렬 getter

	if (ShowPrimitives == EEngineShowFlags::SF_Primitives)
		GetSceneManager().GetScene()->Render();

	// 기즈모 그리기
	gizmoManager.Draw(GetRenderer());


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Batch Rendering
	// 여기서 라인들 초기화
	LineBatcherManager.BeginFrame();

	// 1) aabb 쌓기 : 메쉬에 저장된 기본 bounds를 가져온다
	UPrimitiveComponent* PickedPrimitive = gizmoManager.GetTarget();

	// 픽 된 메쉬가 존재할때만
	if (PickedPrimitive != nullptr)
	{
		UMesh* Mesh = PickedPrimitive->GetMesh();
		if (Mesh == nullptr)
		{
			return; // 처리할 메쉬나 정점이 없으면 종료
		}

		FBounds WorldBounds;
		FMatrix WorldMatrix = PickedPrimitive->GetWorldTransform();

		if (PickedPrimitive->GetClass() == USphereComp::StaticClass())
		{
			//ULineBatcherManager::LocalSphereToWorldAABB(PickedPrimitive->GetPosition(), WorldMatrix, WorldBounds);
			LineBatcherManager.AddBoundingBox(WorldBounds, 0xFFFFFFFF);
			//LineBatcherManager.AddBoundingBox(WorldBounds, 0x00FFFFFF);
		}
		else if (PickedPrimitive->GetClass() == USpotLightComponent::StaticClass())
		{
			//ULineBatcherManager::LocalAABBtoWorldAABB(Mesh->GetLocalBounds(), WorldMatrix, WorldBounds);
			LineBatcherManager.AddSpotLight(PickedPrimitive->GetPosition(), PickedPrimitive->GetWorldTransform(), 15, 3);
		}
		else if (PickedPrimitive->GetClass() == UCubeComp::StaticClass() || PickedPrimitive->GetClass() == UPlaneComp::StaticClass())
		{
			//ULineBatcherManager::LocalAABBtoWorldAABB(Mesh->GetLocalBounds(), WorldMatrix, WorldBounds);
		}
		else
		{
			// 모든 버텍스에 정확한 AABB 박스 생성 (매 프레임 모든 버텍스 순회)
			WorldBounds = Mesh->CalculateAccurateWorldBounds(Mesh, WorldMatrix);
		}

		WorldBounds = Mesh->CalculateAccurateWorldBounds(Mesh, WorldMatrix);
		LineBatcherManager.AddBoundingBox(WorldBounds, 0xFFFFFFFF);

		if (ShowBillboard == EEngineShowFlags::SF_BillboardText)
		{
			UUIDRenderer.SetUUIDVertices(
				Device,
				(float)windowWidth / (float)windowHeight,
				PickedPrimitive->UUID,
				0.05f,
				PickedPrimitive->GetScale().Z,
				WorldMatrix,
				View,
				Proj);
			UUIDRenderer.Bind(DeviceContext);
			UUIDRenderer.Render(DeviceContext);
		}
	}

	// 2) 그리드 쌓기 (원하는 색/간격/개수)
	const int GridCount = 100;
	const uint32_t ColMain = 0xFFFFFF60; // RGBA
	const uint32_t ColAxis = 0xFFFFFFFF;
	LineBatcherManager.AddGrid(LineBatcherManager.GridSpacing, GridCount, ColMain, ColAxis);

	//LineBatcherManager.AddSpotLight({0,0,0}, FMatrix::Identity, 15, 3);
	LineBatcherManager.Render(DeviceContext, View, Proj);
	///////////////////////////////////////////////////////////////////////////////////////////////
}

void EditorApplication::RenderGUI()
{
	controlPanel->Render();
	propertyWindow->Render();
	SceneManagerWindow->Render();
	ToggleWindow->Render();

	ImGui::SetNextWindowPos(ImVec2(0, 500));         // Fixed position (x=20, y=20)
	ImGui::SetNextWindowSize(ImVec2(275, 75));      // Fixed size (width=300, height=100)
	ImGui::Begin("Memory Stats", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse);                // Prevent resizing, moving, collapsing

	ImGui::Text("Allocated Object Count : %d", UEngineStatics::GetTotalAllocationCount());
	ImGui::Text("Allocated Object Bytes : %d", UEngineStatics::GetTotalAllocationBytes());

	ImGui::End();

	bool isConsoleOpen = false;
	// static ImguiConsole imguiConsole;
	GConsole.Draw("Console", &isConsoleOpen);
}

bool EditorApplication::OnInitialize()
{
	Application::OnInitialize();
	// 리사이즈/초기화

	controlPanel = new UControlPanel(&GetSceneManager(), &gizmoManager, &GetLineBatcherManager());
	propertyWindow = new USceneComponentPropertyWindow();
	SceneManagerWindow = new USceneManagerWindow(
		&GetSceneManager(),
		[this](UPrimitiveComponent* UTemp)
		{
			SetTarget(UTemp);
		});
	ToggleWindow = new UToggleWindow();

	if (!gizmoManager.Initialize(&GetMeshManager()))
	{
		MessageBox(GetWindowHandle(), L"Failed to initialize gizmo manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}
	gizmoManager.SetCamera(GetSceneManager().GetScene()->GetCamera());

	return true;
}


void EditorApplication::OnResize(int32 width, int32 height)
{
	UScene* scene = GetSceneManager().GetScene();
	if (scene == nullptr) return;

	UCamera* camera = scene->GetCamera();
	if (camera == nullptr) return;

	camera->SetPerspectiveDegrees(
		camera->GetFOV(),
		(height > 0) ? (float)width / (float)height : 1.0f,
		camera->GetNearZ(),
		camera->GetFarZ());
}

UScene* EditorApplication::CreateDefaultScene()
{
	return new UDefaultScene();
}

void EditorApplication::OnSceneChange()
{
	SetTarget(nullptr);

	gizmoManager.SetCamera(GetSceneManager().GetScene()->GetCamera());
}

void EditorApplication::SetTarget(UPrimitiveComponent* Target)
{
	propertyWindow->SetTarget(Target);
	SceneManagerWindow->SetTarget(Target);
	gizmoManager.SetTarget(Target);
}
