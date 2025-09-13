#include "stdafx.h"
#include "USceneManagerWindow.h"
#include "USceneManager.h"
#include "UScene.h"

void USceneManagerWindow::RenderContent()
{
	const TArray<USceneComponent*>& SceneObjects = SceneManager->GetScene()->GetObjects();

	// 여기서 SceneObjects 목록을 순회하며 모든 USceneComponent Name을 출력하는 리스트를 만들어줘

	// 고유 ID "SceneObjectsList"를 가진 자식 창을 생성합니다.
	// 세로 크기를 200으로 지정하고, 크기를 넘어가면 스크롤바가 생깁니다.
	ImGui::BeginChild("SceneObjectsList", ImVec2(0, 200), true);

	// Scene에 있는 모든 USceneComponent를 순회합니다.
	for (USceneComponent* SceneObject : SceneObjects)
	{
		if (SceneObject == nullptr)
		{
			continue;
		}

		// ImGui의 ID 스택에 각 SceneObject의 고유한 UUID를 PUSH합니다.
		// UUID가 정수 타입(int, uint32 등)이라고 가정합니다.
		ImGui::PushID(SceneObject->UUID);

		// 이제 ImGui::Selectable은 내부적으로 [UUID + 이름] 조합의 고유 ID를 갖게 됩니다.
		// 따라서 이름이 같더라도 ID 충돌이 발생하지 않습니다.
		// FName::ToString()은 FString을 반환하므로, TCHAR_TO_UTF8로 변환해주는 것이 안전합니다.
		if (ImGui::Selectable(SceneObject->Name.ToString().c_str(), SceneObject == Target))
		{
			UPrimitiveComponent* TargetSceneObject = SceneObject->Cast<UPrimitiveComponent>();
			OnComponentSelected(TargetSceneObject);;
		}

		// 현재 ScenObject에 대한 UI 처리가 끝났으므로 스택에서 ID를 POP합니다.
		// PushID와 PopID는 반드시 쌍으로 호출되어야 합니다.
		ImGui::PopID();
	}
	ImGui::EndChild();

	ImGui::Separator(); // 리스트와 속성 편집기 사이에 구분선을 추가하여 가독성을 높입니다.
}
