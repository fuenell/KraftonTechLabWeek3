#include "stdafx.h"
#include "UControlPanel.h"
#include "USceneComponent.h"
#include "UCamera.h"
#include "USceneManager.h"
#include "UScene.h"
#include "UDefaultScene.h"

// 활성화(선택) 상태면 버튼색을 Active 계열로 바꿔서 '눌린 버튼'처럼 보이게 하는 헬퍼
static bool ModeButton(const char* label, bool active, const ImVec2& size = ImVec2(0, 0))
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 colBtn = active ? style.Colors[ImGuiCol_ButtonActive] : style.Colors[ImGuiCol_Button];
	ImVec4 colHover = active ? style.Colors[ImGuiCol_ButtonActive] : style.Colors[ImGuiCol_ButtonHovered];
	ImVec4 colActive = style.Colors[ImGuiCol_ButtonActive];

	ImGui::PushStyleColor(ImGuiCol_Button, colBtn);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colHover);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, colActive);
	bool pressed = ImGui::Button(label, size);
	ImGui::PopStyleColor(3);
	return pressed;
}

UControlPanel::UControlPanel(USceneManager* InSceneManager, UGizmoManager* InGizmoManager, ULineBatcherManager* InLineBatcherManager, URenderer* InRenderer)
	: ImGuiWindowWrapper("Control Panel", ImVec2(0, 0), ImVec2(275, 390)), SceneManager(InSceneManager), GizmoManager(InGizmoManager), LineBatcherManager(InLineBatcherManager), Renderer(InRenderer)
{
	for (const auto& Pair : UClass::GetClassPool())
	{
		UClass* registeredType = Pair.second.get();
		if (!registeredType->IsChildOrSelfOf(USceneComponent::StaticClass()))
			continue;

		FString TypeName = registeredType->GetMeta("TypeName");
		if (TypeName.empty())
			continue;

		RegisteredTypes.push_back(registeredType);
		ChoiceStrList.push_back(registeredType->GetMeta("TypeName"));
	}

	for (const FString& str : ChoiceStrList)
	{
		Choices.push_back(str.c_str());
	}
}

void UControlPanel::RenderContent()
{
	PrimaryInformationSection();
	ImGui::Separator();
	SpawnPrimitiveSection();
	ImGui::Separator();
	SceneManagementSection();
	ImGui::Separator();
	CameraManagementSection();

	// 그리드 조절
	ImGui::Separator();
	GridManagementSection();

	// View Mode
	ImGui::Separator();
	ViewModeSection();
}

void UControlPanel::PrimaryInformationSection()
{
	float frameRate = ImGui::GetIO().Framerate;
	ImGui::Text("FPS %.0f (%.0f ms)", frameRate, 1000.0f / frameRate);
}

USceneComponent* UControlPanel::CreateSceneComponentFromChoice(int index)
{
	auto obj = RegisteredTypes[PrimitiveChoiceIndex]->CreateDefaultObject();
	if (!obj) return nullptr;
	return obj->Cast<USceneComponent>();
}

void UControlPanel::SpawnPrimitiveSection()
{
	ImGui::SetNextItemWidth(150);
	ImGui::Combo("Type", &PrimitiveChoiceIndex, Choices.data(), static_cast<int32>(Choices.size()));

	int32 ObjectCount = SceneManager->GetScene()->GetObjectCount();
	if (ImGui::Button("Spawn"))
	{
		USceneComponent* SceneComponent = CreateSceneComponentFromChoice(PrimitiveChoiceIndex);

		if (SceneComponent == nullptr)
		{
			return;
		}

		FName BaseName = RegisteredTypes[PrimitiveChoiceIndex]->GetUClassName().ToString(); // 이 부분은 엔진의 구현에 따라 std::string을 반환한다고 가정
		FName NewName = BaseName;

		for (UINT i = 1; i < UINT_MAX; i++)
		{
			if (SceneManager->GetScene()->IsNameDuplicated(NewName) == false)
			{
				break;
			}

			NewName = BaseName.ToString() + "_" + std::to_string(i);
		}

		SceneComponent->Name = NewName;

		// 최초 생성
		if (SceneComponent->Name == BaseName)
		{
			SceneComponent->SetPosition(FVector());
			SceneComponent->SetScale(FVector(1, 1, 1));
			SceneComponent->SetRotation(FVector());
		}
		else
		{
			SceneComponent->SetPosition(FVector(
				Random::GetFloat(-2.5f, 2.5f),
				Random::GetFloat(-2.5f, 2.5f),
				Random::GetFloat(-2.5f, 2.5f)
			));
			SceneComponent->SetScale(FVector(
				Random::GetFloat(0.1f, 2.0f),
				Random::GetFloat(0.1f, 2.0f),
				Random::GetFloat(0.1f, 2.0f)
			));
			SceneComponent->SetRotation(FVector(
				Random::GetFloat(-360.0f, 360.0f),
				Random::GetFloat(-360.0f, 360.0f),
				Random::GetFloat(-360.0f, 360.0f)
			));
		}

		SceneManager->GetScene()->AddObject(SceneComponent);
	}

	ImGui::SameLine();
	ImGui::BeginDisabled();
	ImGui::SetNextItemWidth(60);
	ImGui::SameLine();
	ImGui::InputInt("Spawned", &ObjectCount, 0);
	ImGui::EndDisabled();
}

void UControlPanel::SceneManagementSection()
{
	ImGui::Text("Scene Name");                // Label on top
	ImGui::SetNextItemWidth(200);             // Optional: set input width
	ImGui::InputText("##SceneNameInput", SceneManager->SceneName, sizeof(SceneManager->SceneName)); // invisible label

	if (ImGui::Button("New scene"))
	{
		// TODO : Make New Scene
		SceneManager->SetScene(new UDefaultScene());

	}
	ImGui::SameLine();
	if (ImGui::Button("Save scene") && strcmp(SceneManager->SceneName, "") != 0)
	{
		std::filesystem::path _path(SceneManager->SavePath);
		std::filesystem::create_directory(_path);
		SceneManager->SaveScene(_path.string() + FString(SceneManager->SceneName) + SceneManager->SceneExtension);
	}
	ImGui::SameLine();
	if (ImGui::Button("Load scene") && strcmp(SceneManager->SceneName, "") != 0)
	{
		UScene* NewScene = SceneManager->LoadScene(SceneManager->SavePath + FString(SceneManager->SceneName) + SceneManager->SceneExtension);
		SceneManager->SetScene(NewScene);
	}
}

void UControlPanel::CameraManagementSection()
{
	UCamera* camera = SceneManager->GetScene()->GetCamera();
	// 카메라 정보
	FVector pos = camera->GetLocation();
	float cameraLocation[3] = { pos.X, pos.Y, pos.Z };
	FVector eulDeg = camera->GetEulerXYZDeg();
	float eulerXYZ[3] = { eulDeg.X, eulDeg.Y, eulDeg.Z };

	float RotationSensitivity = UCamera::GetRotationSensitivity() * 200.0f;
	float TranslationSensitivity = UCamera::GetTranslationSensitivity();

	// --- 테이블 UI ---
	bool locCommitted = false;
	bool rotCommitted = false;
	bool RotSensCommitted = false;
	bool TransSensCommitted = false;

	bool isOrthogonal = camera->IsOrtho();
	ImGui::Checkbox("Orthogonal", &isOrthogonal);
	if (isOrthogonal)
	{
		// 원하는 직교 크기로 (예시: 월드 단위 10x10)
		camera->SetOrtho(10.0f, 10.0f, camera->GetNearZ(), camera->GetFarZ(), /*leftHanded=*/false);
	}
	else
	{
		camera->SetPerspectiveDegrees(camera->GetFOV(),
			camera->GetAspect(), camera->GetNearZ(), camera->GetFarZ());
	}

	// === FOV (perspective일 때만 활성화) ===
	ImGui::TextUnformatted("FOV");

	float fovDeg = camera->GetFOV();
	bool changed = false;

	const float minFov = 10.0f;
	const float maxFov = 120.0f;
	const float dragSpeed = 0.2f; // 픽셀당 증가량(°)

	float avail = ImGui::GetContentRegionAvail().x;

	ImGui::BeginDisabled(isOrthogonal);
	ImGui::PushID("FOV");

	// 드래그 박스 (좌우 드래그로 값 변경)
	ImGui::SetNextItemWidth(avail * 0.55f);
	changed |= ImGui::DragFloat("##fov_drag", &fovDeg, dragSpeed, minFov, maxFov, "%.1f",
		ImGuiSliderFlags_AlwaysClamp);

	// 직접 입력 박스(원하면 유지, 아니면 생략 가능)
	ImGui::SameLine();
	ImGui::SetNextItemWidth(avail * 0.30f);
	changed |= ImGui::InputFloat("##fov_input", &fovDeg, 0.0f, 0.0f, "%.3f");

	// 리셋 버튼(옵션)
	ImGui::SameLine();
	if (ImGui::Button("Reset")) { fovDeg = 60.0f; changed = true; }

	if (changed)
	{
		fovDeg = std::clamp(fovDeg, minFov, maxFov);
		camera->SetFOV(fovDeg); // 내부에서 proj 재빌드
	}

	ImGui::PopID();
	ImGui::EndDisabled();



	// --- Euler(XYZ) 편집 ---
	// Location
	ImGui::Text("Camera Location");
	if (ImGui::BeginTable("EditableCameraTable", 3, ImGuiTableFlags_None))
	{
		ImGui::TableNextRow();
		for (int32 i = 0; i < 3; i++)
		{
			ImGui::TableSetColumnIndex(i);
			ImGui::SetNextItemWidth(-1);

			// DragFloat로 교체
			if (ImGui::DragFloat(("##loc" + std::to_string(i)).c_str(),
				&cameraLocation[i], 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
			{
				locCommitted = true; // 값이 바뀐 순간 바로 commit
			}
			// 만약 "편집 종료 후만" commit 원하면 IsItemDeactivatedAfterEdit() 체크로 바꾸기
		}
		ImGui::EndTable();
	}

	ImGui::Text("Camera Rotation");
	if (ImGui::BeginTable("CameraRotTable", 3, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
	{
		ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
		ImGui::TableSetColumnIndex(0); ImGui::Text("Pitch");
		ImGui::TableSetColumnIndex(1); ImGui::Text("Yaw");
		ImGui::TableSetColumnIndex(2); ImGui::Text("Roll");

		// Camera Rotation 행
		ImGui::TableNextRow();
		for (int32 i = 0; i < 3; i++)
		{
			ImGui::TableSetColumnIndex(i);
			ImGui::SetNextItemWidth(-1);
			ImGui::InputFloat(("##rot" + std::to_string(i)).c_str(),
				&eulerXYZ[i], 0.0f, 0.0f, "%.3f");
			if (ImGui::IsItemDeactivatedAfterEdit())
				rotCommitted = true;
		}
		// ImGui::TableSetColumnIndex(3);

		ImGui::EndTable();
	}

	// 카메라 회전 감도 조작
	ImGui::Text("Rotation Sensitivity");

	const float RotationSensitivityMin = 0.1f;
	const float RotationSensitivityMax = 2.0f;
	// DragFloat로 교체
	if (ImGui::DragFloat("##RotationSensitivity",
		&RotationSensitivity, 0.1f, RotationSensitivityMin, RotationSensitivityMax, "%.3f"))
	{
		RotSensCommitted = true; // 값이 바뀐 순간 바로 commit
	}

	// 카메라 이동 감도 조작
	ImGui::Text("Translation Sensitivity");

	const float TranslationSensitivityMin = 0.1f;
	const float TranslationSensitivityMax = 10.0f;
	// DragFloat로 교체
	if (ImGui::DragFloat("##TranslationSensitivity",
		&TranslationSensitivity, 0.1f, TranslationSensitivityMin, TranslationSensitivityMax, "%.3f"))
	{
		TransSensCommitted = true; // 값이 바뀐 순간 바로 commit
	}

	// World / Local 선택 (체크박스 대신, 버튼처럼 보이는 상호배타 토글)

	if (ModeButton("World", GizmoManager->GetIsWorldSpace()))
	{
		GizmoManager->SetGizmoSpace(true);
	}
	ImGui::SameLine();
	if (ModeButton("Local", !GizmoManager->GetIsWorldSpace()))
	{
		GizmoManager->SetGizmoSpace(false);
	}
	/* 모드/좌표계 토글 렌더링 */
	if (ModeButton("Translation", GizmoManager->GetTranslationType() == ETranslationType::Location))
	{
		GizmoManager->SetTranslationType(ETranslationType::Location);
	}
	ImGui::SameLine();
	if (ModeButton("Rotation", GizmoManager->GetTranslationType() == ETranslationType::Rotation))
	{
		GizmoManager->SetTranslationType(ETranslationType::Rotation);
	}
	ImGui::SameLine();
	if (ModeButton("Scale", GizmoManager->GetTranslationType() == ETranslationType::Scale))
	{
		GizmoManager->SetTranslationType(ETranslationType::Scale);
	}

	// === 변경사항을 카메라에 반영 ===
	// 위치
	// === 변경사항을 카메라에 '커밋 시'만 반영 ===
	if (locCommitted)
	{
		camera->SetLocation(FVector(cameraLocation[0], cameraLocation[1], cameraLocation[2]));
	}

	if (rotCommitted)
	{
		camera->SetEulerXYZDeg(eulerXYZ[0], eulerXYZ[1], eulerXYZ[2]);
	}

	if (RotSensCommitted)
	{
		UCamera::SetRotationSensitivity(RotationSensitivity / 200.0f);
	}

	if (TransSensCommitted)
	{
		UCamera::SetTranslationSensitivity(TranslationSensitivity);
	}
}

void UControlPanel::GridManagementSection()
{
	if (!LineBatcherManager) return;

	// 실제 그리드 간격 가져오기
	float gridSpace = LineBatcherManager->GetGridSpace();

	ImGui::Text("Grid Spacing");
	if (ImGui::BeginTable("GridTable", 1, ImGuiTableFlags_None))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::SetNextItemWidth(-1);

		// 카메라 Location과 동일하게: Drag 중 변경되면 즉시 커밋
		if (ImGui::DragFloat("##GridSpacing", &gridSpace, 0.5f, 0.001f, FLT_MAX, "%.3f"))
		{
			// 방어적 클램프 (0 이하 방지)
			gridSpace = gridSpace < 1.0f ? 1.0f : gridSpace;
			LineBatcherManager->SetGridSpace(gridSpace);
		}

		ImGui::EndTable();
	}
}

void UControlPanel::ViewModeSection()
{
	ImGui::Text("View Mode");

	int32_t Mode = static_cast<int>(Renderer->GetViewModeIndex());
	bool bChanged = false;

	bChanged |= ImGui::RadioButton("Lit", &Mode, static_cast<int32_t>(EViewModeIndex::VMI_Lit));
	ImGui::SameLine();
	bChanged |= ImGui::RadioButton("Unlit", &Mode, static_cast<int32_t>(EViewModeIndex::VMI_Unlit));
	ImGui::SameLine();
	bChanged |= ImGui::RadioButton("Wireframe", &Mode, static_cast<int32_t>(EViewModeIndex::VMI_Wireframe));

	if (bChanged)
	{
		Renderer->SetRasterizerState(static_cast<EViewModeIndex>(Mode));
	}
}

