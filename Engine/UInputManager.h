#pragma once
#include "stdafx.h"

class UInputManager
{
private:
    // Key states
    bool keyStates[256];
    bool prevKeyStates[256];

    // Mouse states
    bool mouseButtons[3]; // Left, Right, Middle
    bool prevMouseButtons[3];
    int mouseX, mouseY;

public:
    UInputManager();
    ~UInputManager();

    // Frame management
    void Update();
    bool ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Keyboard input
    bool IsKeyDown(int keyCode) const;
    bool IsKeyPressed(int keyCode) const;  // True only on the frame key was pressed
    bool IsKeyReleased(int keyCode) const; // True only on the frame key was released

    // Mouse input
    bool IsMouseButtonDown(int button) const; // 0=Left, 1=Right, 2=Middle
    bool IsMouseButtonPressed(int button) const;
    bool IsMouseButtonReleased(int button) const;

    // Mouse position and movement
    int GetMouseX() const { return mouseX; }
    int GetMouseY() const { return mouseY; }
    // Utility
    void ResetStates();

private:
    void HandleKeyDown(WPARAM wParam);
    void HandleKeyUp(WPARAM wParam);
    void HandleMouseMove(LPARAM lParam);
    void HandleMouseButton(UINT message, WPARAM wParam);
    void HandleMouseWheel(WPARAM wParam);
};