
// UToggleWindow.cpp
#include "stdafx.h"
#include "ToggleWindow.h"

/* private functions */
UToggleWindow::UToggleWindow()
    : ImGuiWindowWrapper("Toggle Window", ImVec2(275, 250), ImVec2(200, 150))
{
}

void UToggleWindow::RenderContent()
{
    bool bShowPrimitives;
    bool bShowBillboardText;

    // Show Flags Section
    if (ShowPrimitives == EEngineShowFlags::SF_Primitives)
        bShowPrimitives = true;
    else
        bShowPrimitives = false;

    if (ShowBillboard == EEngineShowFlags::SF_BillboardText)
        bShowBillboardText = true;
    else
        bShowBillboardText = false;

    ImGui::Text("Render Toggles");
    ImGui::Spacing();

    // 체크박스 스타일
    if (ImGui::Checkbox("Primitives", &bShowPrimitives))
    {
        if (bShowPrimitives)
            ShowPrimitives = EEngineShowFlags::SF_Primitives;
        else
            ShowPrimitives = EEngineShowFlags::SF_None;
    }

    if (ImGui::Checkbox("BillboardText", &bShowBillboardText))
    {
        if (bShowBillboardText)
            ShowBillboard = EEngineShowFlags::SF_BillboardText;
        else
            ShowBillboard = EEngineShowFlags::SF_None;
    }
}
