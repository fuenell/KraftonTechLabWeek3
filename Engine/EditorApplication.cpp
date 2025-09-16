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
	USceneManager::GetInstance().GetScene()->Update(DeltaTime);

	GizmoManager.Update(DeltaTime);

	ProcessKeyboardInput();

	ProcessMouseInteraction();
}

void EditorApplication::ProcessKeyboardInput()
{
	if (UInputManager::GetInstance().IsKeyDown(VK_ESCAPE))
	{
		RequestExit();
	}
	if (UInputManager::GetInstance().IsKeyPressed(VK_SPACE))
	{
		GizmoManager.NextTranslation();
	}
	if (UInputManager::GetInstance().IsKeyPressed('X'))
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
		if (UInputManager::GetInstance().IsMouseButtonReleased(0))
		{
			GizmoManager.EndDrag();
		}
		// 드래그가 계속되고 있다면 마우스 위치에 따라 기즈모를 업데이트합니다.
		else
		{
			FRay Ray = URaycastManager::GetInstance().CreateRayFromScreenPosition(USceneManager::GetInstance().GetScene()->GetCamera());
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
		if (UInputManager::GetInstance().IsMouseButtonPressed(0))
		{
			FVector OutImpactPoint;
			UGizmoComponent* HitGizmo = nullptr;
			UPrimitiveComponent* HitPrimitive = nullptr;

			// 씬의 모든 PrimitiveComponent와 GizmoComponent를 수집합니다.
			TArray<UPrimitiveComponent*> Primitives;
			for (UObject* Obj : USceneManager::GetInstance().GetScene()->GetObjects())
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
			if (URaycastManager::GetInstance().RayIntersectsMeshes(USceneManager::GetInstance().GetScene()->GetCamera(), Gizmos, HitGizmo, OutImpactPoint))
			{
				HitGizmo->bIsSelected = true;
				FRay Ray = URaycastManager::GetInstance().CreateRayFromScreenPosition(USceneManager::GetInstance().GetScene()->GetCamera());

				// 히트된 기즈모의 종류에 따라 적절한 드래그를 시작합니다.
				if (UGizmoArrowComp* Arrow = HitGizmo->Cast<UGizmoArrowComp>())
				{
					GizmoManager.BeginDrag(Ray, Arrow->Axis, OutImpactPoint, USceneManager::GetInstance().GetScene());
				}
				else if (UGizmoRotationHandleComp* rotationHandle = HitGizmo->Cast<UGizmoRotationHandleComp>())
				{
					GizmoManager.BeginDrag(Ray, rotationHandle->Axis, OutImpactPoint, USceneManager::GetInstance().GetScene());
				}
				else if (UGizmoScaleHandleComp* scaleHandle = HitGizmo->Cast<UGizmoScaleHandleComp>())
				{
					GizmoManager.BeginDrag(Ray, scaleHandle->Axis, OutImpactPoint, USceneManager::GetInstance().GetScene());
				}
			}
			// 2. 기즈모가 아니라면, 일반 오브젝트에 대한 피킹을 시도합니다.
			else if (URaycastManager::GetInstance().RayIntersectsMeshes(USceneManager::GetInstance().GetScene()->GetCamera(), Primitives, HitPrimitive, OutImpactPoint))
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
	ID3D11Device* Device = URenderer::GetInstance().GetDevice();
	ID3D11DeviceContext* DeviceContext = URenderer::GetInstance().GetDeviceContext();

	const FMatrix View = USceneManager::GetInstance().GetScene()->GetCamera()->GetView();   // 네 쪽의 뷰 행렬 getter
	const FMatrix Proj = USceneManager::GetInstance().GetScene()->GetCamera()->GetProj();   // 네 쪽의 프로젝션 행렬 getter

	if (ShowPrimitives == EEngineShowFlags::SF_Primitives)
	{
		// 프리미티브 그리기
		USceneManager::GetInstance().GetScene()->Render();

		// 기즈모 그리기
		GizmoManager.Draw(URenderer::GetInstance());
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Batch Rendering
	// 여기서 라인들 초기화
	ULineBatcherManager::GetInstance().BeginFrame();

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
			ULineBatcherManager::GetInstance().AddBoundingBox(WorldBounds, 0xFFFFFFFF);
		}
		else if (PickedPrimitive->GetClass() == USpotLightComponent::StaticClass())
		{
			USpotLightComponent* SpotLightComponent = dynamic_cast<USpotLightComponent*>(PickedPrimitive);
			ULineBatcherManager::LocalAABBtoWorldAABB(Mesh->GetLocalBounds(), WorldMatrix, WorldBounds);
			ULineBatcherManager::GetInstance().AddSpotLight(PickedPrimitive->GetPosition(), PickedPrimitive->GetWorldTransform(), SpotLightComponent->GetAngle(), SpotLightComponent->GetScale(), SpotLightComponent->GetLightColor());
		}
		else if (PickedPrimitive->GetClass() == UCubeComp::StaticClass() || PickedPrimitive->GetClass() == UPlaneComp::StaticClass())
		{
			ULineBatcherManager::LocalAABBtoWorldAABB(Mesh->GetLocalBounds(), WorldMatrix, WorldBounds);
			ULineBatcherManager::GetInstance().AddBoundingBox(WorldBounds, 0xFFFFFFFF);
		}
		else
		{
			// 모든 버텍스에 정확한 AABB 박스 생성 (매 프레임 모든 버텍스 순회)
			Mesh->UpdateAccurateBounds(WorldMatrix);
			WorldBounds = Mesh->GetAccurateBounds();
			ULineBatcherManager::GetInstance().AddBoundingBox(WorldBounds, 0xFFFFFFFF);
		}
	}

	// 2) 그리드 쌓기 (원하는 색/간격/개수)
	const int GridCount = 100;
	const uint32_t ColMain = 0xFFFFFF60; // RGBA
	const uint32_t ColAxis = 0xFFFFFFFF;
	ULineBatcherManager::GetInstance().AddGrid(ULineBatcherManager::GetInstance().GridSpacing, GridCount, ColMain, ColAxis);


	ULineBatcherManager::GetInstance().Render(DeviceContext, View, Proj);

	//
	///////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Sprite(BillBoard) Rendering
	if (PickedPrimitive != nullptr)
	{
		USpriteManager::GetInstance().BeginFrame();

		FMatrix WorldMatrix = PickedPrimitive->GetWorldTransform();

		if (ShowBillboard == EEngineShowFlags::SF_BillboardText)
		{
			if (USpriteManager::GetInstance().SetUUIDVertices(Device, (float)WindowWidth / (float)WindowHeight, PickedPrimitive->UUID, 0.05f, PickedPrimitive->GetScale().Z, WorldMatrix, View, Proj))
			{
				USpriteManager::GetInstance().Bind(DeviceContext);
				USpriteManager::GetInstance().Render(DeviceContext);
			}
		}



	}
	//
	///////////////////////////////////////////////////////////////////////////////////////////////

	USubUVManager& SubUVManager = USubUVManager::GetInstance();

	SubUVManager.UpdateConstantBuffer(
		FVector(1.0f, 1.0f, 1.0f),
		FVector(1.0f, 1.0f, 1.0f),
		USceneManager::GetInstance().GetScene()->GetCamera()->GetRotation().ToMatrixRow(),
		View,
		Proj
	);
	SubUVManager.Bind();
	SubUVManager.Render();
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
	ImGui::Text("Allocated Object Size : %.2f KB", static_cast<float>(UEngineStatics::GetTotalAllocationBytes()) / 1024.0f);

	ImGui::End();

	bool bIsConsoleOpen = false;
	// static ImguiConsole imguiConsole;
	GConsole.Draw("Console", &bIsConsoleOpen);
}

bool EditorApplication::OnInitialize()
{
	// 리사이즈/초기화

	ControlPanel = new UControlPanel(&USceneManager::GetInstance(), &GizmoManager, &ULineBatcherManager::GetInstance(), &URenderer::GetInstance());
	PropertyWindow = new USceneComponentPropertyWindow();
	SceneManagerWindow = new USceneManagerWindow(
		&USceneManager::GetInstance(),
		[this](UPrimitiveComponent* UTemp)
		{
			SetTarget(UTemp);
		});
	ToggleWindow = new UToggleWindow();

	if (!GizmoManager.Initialize(&UMeshManager::GetInstance()))
	{
		MessageBox(GetWindowHandle(), L"Failed to initialize gizmo manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}
	GizmoManager.SetCamera(USceneManager::GetInstance().GetScene()->GetCamera());

	return true;
}


void EditorApplication::OnResize(int32 Width, int32 Height)
{
	UScene* Scene = USceneManager::GetInstance().GetScene();
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

	GizmoManager.SetCamera(USceneManager::GetInstance().GetScene()->GetCamera());
}

void EditorApplication::SetTarget(UPrimitiveComponent* Target)
{
	PropertyWindow->SetTarget(Target);
	SceneManagerWindow->SetTarget(Target);
	GizmoManager.SetTarget(Target);
}
