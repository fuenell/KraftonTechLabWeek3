#pragma once
#include "stdafx.h"
#include "UEngineStatics.h"
#include <string>

class ImGuiWindowWrapper
{
private:
    FString WindowName;

public:
    ImGuiWindowWrapper(FString windowName) : WindowName(windowName) {}

    void Render()
    {
        if (ImGui::Begin(WindowName.c_str()))
        {
            RenderContent();
        }
        ImGui::End();
    }

protected:
    virtual void RenderContent() = 0;  // 파생 클래스에서 구현
};