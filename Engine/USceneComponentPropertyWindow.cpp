#include "stdafx.h"
#include "USceneComponentPropertyWindow.h"
#include "USpotLightComponent.h"

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

void USceneComponentPropertyWindow::RenderContent()
{
	if (!Target) return;

	// ── 공통: TRS 편집 테이블 ───────────────────────────────
	FVector& location = Target->RelativeLocation;
	FVector rotation = Target->GetRotation();
	FVector& scale = Target->RelativeScale3D;

	float locationInput[3] = { location.X, location.Y, location.Z };
	float rotationInput[3] = { rotation.X, rotation.Y, rotation.Z };
	float scaleInput[3] = { scale.X,    scale.Y,    scale.Z };
	bool rotCommited = false;
	// 나머지는 테이블로
	if (ImGui::BeginTable("EditablePropertyTable", 4, ImGuiTableFlags_None))
	{
		ImGui::TableNextRow();
		for (int32 i = 0; i < 3; i++)
		{
			ImGui::TableSetColumnIndex(i);
			ImGui::SetNextItemWidth(-1);
			ImGui::InputFloat(("##tra" + std::to_string(i)).c_str(),
				&locationInput[i], 0.0f, 0.0f, "%.3f");
		}
		ImGui::TableSetColumnIndex(3);
		ImGui::Text("Translation");

		ImGui::TableNextRow();
		for (int32 i = 0; i < 3; i++)
		{
			ImGui::TableSetColumnIndex(i);
			ImGui::SetNextItemWidth(-1);
			ImGui::InputFloat(("##rot" + std::to_string(i)).c_str(),
				&rotationInput[i], 0.0f, 0.0f, "%.3f");
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				rotCommited = true;
			}
		}
		ImGui::TableSetColumnIndex(3);
		ImGui::Text("Rotation");

		ImGui::TableNextRow();
		for (int32 i = 0; i < 3; i++)
		{
			ImGui::TableSetColumnIndex(i);
			ImGui::SetNextItemWidth(-1);
			ImGui::InputFloat(("##scl" + std::to_string(i)).c_str(),
				&scaleInput[i], 0.0f, 0.0f, "%.3f");
		}
		ImGui::TableSetColumnIndex(3);
		ImGui::Text("Scale");

		ImGui::EndTable();
	}

	if (rotCommited)
	{
		Target->SetRotation({ rotationInput[0], rotationInput[1], rotationInput[2] });
	}
	location = FVector(locationInput[0], locationInput[1], locationInput[2]);
	rotation = FVector(rotationInput[0], rotationInput[1], rotationInput[2]);
	scale = FVector(scaleInput[0], scaleInput[1], scaleInput[2]);


	// ── 스포트라이트 전용 UI ────────────────────────────────
	if (Target->GetClass() == USpotLightComponent::StaticClass())
	{
		USpotLightComponent* SpotLightComponent = static_cast<USpotLightComponent*>(Target);
		ImGui::SeparatorText("SpotLight");

		// 현재 컬러/각도 가져오기 (FColor(0~255) 기준 가정)
		FVector4 Cur = SpotLightComponent->GetLightColor();                 // A,R,G,B (엔진 구조에 맞춰 변경)
		float  Col[4] = {
			Cur.X / 255.0f, Cur.Y / 255.0f, Cur.Z / 255.0f, Cur.W / 255.0f
		};

		float AngleDeg = SpotLightComponent->GetAngle();      // 도(deg) 단위 가정
		float ScaleLen = SpotLightComponent->GetScale();
		
		// 2열 레이아웃: 좌측 큰 컬러 피커, 우측 라벨/프리뷰
		if (ImGui::BeginTable("SpotLightProps", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Picker", ImGuiTableColumnFlags_WidthStretch, 3.0f);
			ImGui::TableSetupColumn("Side", ImGuiTableColumnFlags_WidthStretch, 1.0f);

			// ─ Left: Color Picker (+ RGB/HSV/Hex 입력 표시)
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGuiColorEditFlags flags =
				ImGuiColorEditFlags_PickerHueBar |
				ImGuiColorEditFlags_DisplayRGB |
				ImGuiColorEditFlags_DisplayHSV |
				ImGuiColorEditFlags_DisplayHex |
				ImGuiColorEditFlags_InputRGB |
				ImGuiColorEditFlags_AlphaBar;

			bool bColorChanged = ImGui::ColorPicker4("##SpotColor", Col, flags);

			// ─ Right: 제목/미리보기 박스
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("Color");
			ImGui::ColorButton("##Preview",
				ImVec4(Col[0], Col[1], Col[2], Col[3]),
				ImGuiColorEditFlags_NoTooltip,
				ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight() * 2.0f));

			// ─ Angle 슬라이더(0~90도 권장)
			ImGui::Dummy(ImVec2(0, 6));
			ImGui::Text("Angle");
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			bool bAngleChanged = ImGui::SliderFloat("##SpotAngle", &AngleDeg, 0.0f, 90.0f, "%.3f");

			ImGui::Dummy(ImVec2(0, 6));
			ImGui::Text("Scale");
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			// 범위는 프로젝트에 맞춰 조정(예: 0.01~1000)
			const bool bScaleChanged = ImGui::SliderFloat("##SpotScale", &ScaleLen, 1.0f, 200.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);


			ImGui::EndTable();

			// 변경 반영
			if (bColorChanged)
			{
				// 0~1 → 0~255로 반올림만 하고 캐스팅 (클램프 없음)
				const uint32_t R = static_cast<uint32_t>(Col[0] * 255.0f + 0.5f);
				const uint32_t G = static_cast<uint32_t>(Col[1] * 255.0f + 0.5f);
				const uint32_t B = static_cast<uint32_t>(Col[2] * 255.0f + 0.5f);
				const uint32_t A = static_cast<uint32_t>(Col[3] * 255.0f + 0.5f);

				UE_LOG("Alpha value : %d ", A);

				const FVector4 NewColor(R, G, B, A);
				SpotLightComponent->SetLightColor(NewColor);
			}
			if (bAngleChanged)
			{
				SpotLightComponent->SetAngle(AngleDeg);
			}
			if (bScaleChanged)
			{
				SpotLightComponent->SetScale(ScaleLen); 
			}
		}
	}
}
