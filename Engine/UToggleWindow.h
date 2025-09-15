#pragma once

#include "ImGuiWindowWrapper.h"
#include "ShowFlag.h"

class UToggleWindow : public ImGuiWindowWrapper
{

public:
    UToggleWindow();
    void RenderContent() override;
};
