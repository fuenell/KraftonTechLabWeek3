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

void EditorApplication::Update(float DeltaTime)
{
	GetSceneManager().GetScene()->Update(DeltaTime);

	GizmoManager.Update(DeltaTime);

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
		GizmoManager.NextTranslation();
	}
	if (GetInputManager().IsKeyPressed('X'))
	{
		GizmoManager.ChangeGizmoSpace();
	}
}

void EditorApplication::ProcessMouseInteraction()
{
	// =================================================================
	// 상태 1: 기즈모를 드래그하고 있는 중일 때의 로직
	// =================================================================
	if (GizmoManager.IsDragging())
	{
		// 마우스 왼쪽 버튼을 떼면 드래그를 종료합니다.
		if (GetInputManager().IsMouseButtonReleased(0))
		{
			GizmoManager.EndDrag();
		}
		// 드래그가 계속되고 있다면 마우스 위치에 따라 기즈모를 업데이트합니다.
		else
		{
			FRay Ray = GetRaycastManager().CreateRayFromScreenPosition(GetSceneManager().GetScene()->GetCamera());
			GizmoManager.UpdateDrag(Ray);
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
			FVector OutImpactPoint;
			UGizmoComponent* HitGizmo = nullptr;
			UPrimitiveComponent* HitPrimitive = nullptr;

			// 씬의 모든 PrimitiveComponent와 GizmoComponent를 수집합니다.
			TArray<UPrimitiveComponent*> Primitives;
			for (UObject* Obj : GetSceneManager().GetScene()->GetObjects())
			{
				if (UPrimitiveComponent* Primitive = Obj->Cast<UPrimitiveComponent>())
				{
					if (Primitive->GetMesh()) Primitives.push_back(Primitive);
					Primitive->bIsSelected = false;
				}
			}

			TArray<UGizmoComponent*> Gizmos;
			TArray<UGizmoComponent*>& G = GizmoManager.GetRaycastableGizmos();
			if (G.size() > 0)
			{
				for (UGizmoComponent* gizmo : G)
				{
					Gizmos.push_back(gizmo);
					gizmo->bIsSelected = false;
				}
			}

			// 1. 기즈모에 대한 피킹을 먼저 시도합니다.
			if (GetRaycastManager().RayIntersectsMeshes(GetSceneManager().GetScene()->GetCamera(), Gizmos, HitGizmo, OutImpactPoint))
			{
				HitGizmo->bIsSelected = true;
				FRay Ray = GetRaycastManager().CreateRayFromScreenPosition(GetSceneManager().GetScene()->GetCamera());

				// 히트된 기즈모의 종류에 따라 적절한 드래그를 시작합니다.
				if (UGizmoArrowComp* Arrow = HitGizmo->Cast<UGizmoArrowComp>())
				{
					GizmoManager.BeginDrag(Ray, Arrow->Axis, OutImpactPoint, GetSceneManager().GetScene());
				}
				else if (UGizmoRotationHandleComp* rotationHandle = HitGizmo->Cast<UGizmoRotationHandleComp>())
				{
					GizmoManager.BeginDrag(Ray, rotationHandle->Axis, OutImpactPoint, GetSceneManager().GetScene());
				}
				else if (UGizmoScaleHandleComp* scaleHandle = HitGizmo->Cast<UGizmoScaleHandleComp>())
				{
					GizmoManager.BeginDrag(Ray, scaleHandle->Axis, OutImpactPoint, GetSceneManager().GetScene());
				}
			}
			// 2. 기즈모가 아니라면, 일반 오브젝트에 대한 피킹을 시도합니다.
			else if (GetRaycastManager().RayIntersectsMeshes(GetSceneManager().GetScene()->GetCamera(), Primitives, HitPrimitive, OutImpactPoint))
			{
				HitPrimitive->bIsSelected = true;

				if (HitPrimitive->IsManageable())
				{
					SetTarget(HitPrimitive);
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
	ID3D11Device* Device = Renderer.GetDevice();
	ID3D11DeviceContext* DeviceContext = Renderer.GetDeviceContext();

	const FMatrix View = SceneManager.GetScene()->GetCamera()->GetView();   // 네 쪽의 뷰 행렬 getter
	const FMatrix Proj = SceneManager.GetScene()->GetCamera()->GetProj();   // 네 쪽의 프로젝션 행렬 getter

	if (ShowPrimitives == EEngineShowFlags::SF_Primitives)
	{
		GetSceneManager().GetScene()->Render();
		// 기즈모 그리기
		GizmoManager.Draw(GetRenderer());
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Batch Rendering
	// 여기서 라인들 초기화
	LineBatcherManager.BeginFrame();

	// 1) aabb 쌓기 : 메쉬에 저장된 기본 bounds를 가져온다
	UPrimitiveComponent* PickedPrimitive = GizmoManager.GetTarget();

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
			ULineBatcherManager::LocalSphereToWorldAABB(PickedPrimitive->GetPosition(), WorldMatrix, WorldBounds);
			LineBatcherManager.AddBoundingBox(WorldBounds, 0xFFFFFFFF);
			//LineBatcherManager.AddBoundingBox(WorldBounds, 0x00FFFFFF);
		}
		else if (PickedPrimitive->GetClass() == USpotLightComponent::StaticClass())
		{
			USpotLightComponent* SpotLightComponent = dynamic_cast<USpotLightComponent*>(PickedPrimitive);
			ULineBatcherManager::LocalAABBtoWorldAABB(Mesh->GetLocalBounds(), WorldMatrix, WorldBounds);
			LineBatcherManager.AddSpotLight(PickedPrimitive->GetPosition(), PickedPrimitive->GetWorldTransform(), SpotLightComponent->GetAngle(), SpotLightComponent->GetScale(), SpotLightComponent->GetLightColor());
		}
		else if (PickedPrimitive->GetClass() == UCubeComp::StaticClass() || PickedPrimitive->GetClass() == UPlaneComp::StaticClass())
		{
			ULineBatcherManager::LocalAABBtoWorldAABB(Mesh->GetLocalBounds(), WorldMatrix, WorldBounds);
			LineBatcherManager.AddBoundingBox(WorldBounds, 0xFFFFFFFF);
		}
		else
		{
			// 모든 버텍스에 정확한 AABB 박스 생성 (매 프레임 모든 버텍스 순회)
			WorldBounds = Mesh->CalculateAccurateWorldBounds(Mesh, WorldMatrix);
		}
		//WorldBounds = Mesh->CalculateAccurateWorldBounds(Mesh, WorldMatrix);
		//LineBatcherManager.AddBoundingBox(WorldBounds, 0xFFFFFFFF);

		if (ShowBillboard == EEngineShowFlags::SF_BillboardText)
		{
			if (UUIDRenderer.SetUUIDVertices(
				Device,
				(float)WindowWidth / (float)WindowHeight,
				PickedPrimitive->UUID,
				0.5f,
				PickedPrimitive->GetScale().Z,
				WorldMatrix,
				View,
				Proj)
				)
			{
				UUIDRenderer.Bind(DeviceContext);
				UUIDRenderer.Render(DeviceContext);
			}
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

	SpriteManager.Render(DeviceContext);

}

void EditorApplication::RenderGUI()
{
	ControlPanel->Render();
	PropertyWindow->Render();
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

	bool bIsConsoleOpen = false;
	// static ImguiConsole imguiConsole;
	GConsole.Draw("Console", &bIsConsoleOpen);
}

bool EditorApplication::OnInitialize()
{
	// 리사이즈/초기화

	ControlPanel = new UControlPanel(&GetSceneManager(), &GizmoManager, &GetLineBatcherManager(), &Renderer);
	PropertyWindow = new USceneComponentPropertyWindow();
	SceneManagerWindow = new USceneManagerWindow(
		&GetSceneManager(),
		[this](UPrimitiveComponent* UTemp)
		{
			SetTarget(UTemp);
		});
	ToggleWindow = new UToggleWindow();

	if (!GizmoManager.Initialize(&GetMeshManager()))
	{
		MessageBox(GetWindowHandle(), L"Failed to initialize gizmo manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}
	GizmoManager.SetCamera(GetSceneManager().GetScene()->GetCamera());

	return true;
}


void EditorApplication::OnResize(int32 Width, int32 Height)
{
	UScene* Scene = GetSceneManager().GetScene();
	if (Scene == nullptr) return;

	UCamera* Camera = Scene->GetCamera();
	if (Camera == nullptr) return;

	Camera->SetPerspectiveDegrees(
		Camera->GetFOV(),
		(Height > 0) ? (float)Width / (float)Height : 1.0f,
		Camera->GetNearZ(),
		Camera->GetFarZ());
}

void EditorApplication::OnSceneChange()
{
	SetTarget(nullptr);

	GizmoManager.SetCamera(GetSceneManager().GetScene()->GetCamera());
}

void EditorApplication::SetTarget(UPrimitiveComponent* Target)
{
	PropertyWindow->SetTarget(Target);
	SceneManagerWindow->SetTarget(Target);
	GizmoManager.SetTarget(Target);
}
