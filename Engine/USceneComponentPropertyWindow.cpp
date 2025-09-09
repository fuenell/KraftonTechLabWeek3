#include "stdafx.h"
#include "USceneComponentPropertyWindow.h"

void USceneComponentPropertyWindow::RenderContent()
{
    if (!Target) return;

    FVector& location   = Target->RelativeLocation;
    FVector& rotation   = Target->RelativeRotation;
    FVector& scale      = Target->RelativeScale3D;

    float locationInput[3]  = { location.X, location.Y, location.Z };
    float rotationInput[3]  = { rotation.X, rotation.Y, rotation.Z };
    float scaleInput[3]     = { scale.X,    scale.Y,    scale.Z };

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

    location    = FVector(locationInput[0], locationInput[1],   locationInput[2]);
    rotation    = FVector(rotationInput[0], rotationInput[1],   rotationInput[2]);
    scale       = FVector(scaleInput[0],    scaleInput[1],      scaleInput[2]);
}
