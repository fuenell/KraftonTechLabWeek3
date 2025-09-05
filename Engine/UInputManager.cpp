#include "stdafx.h"
#include "UInputManager.h"

UInputManager::UInputManager()
    : mouseX(0)
    , mouseY(0)
{
    // Initialize all key states to false
    memset(keyStates, 0, sizeof(keyStates));
    memset(prevKeyStates, 0, sizeof(prevKeyStates));

    // Initialize mouse button states
    memset(mouseButtons, 0, sizeof(mouseButtons));
    memset(prevMouseButtons, 0, sizeof(prevMouseButtons));
}

UInputManager::~UInputManager()
{
    // Nothing specific to clean up
}

void UInputManager::Update()
{
    // Copy current states to previous states for next frame comparison
    memcpy(prevKeyStates, keyStates, sizeof(keyStates));
    memcpy(prevMouseButtons, mouseButtons, sizeof(mouseButtons));

}

bool UInputManager::ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        HandleKeyDown(wParam);
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        HandleKeyUp(wParam);
        break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        HandleMouseButton(msg, wParam);
        // Fall through to handle mouse position
    case WM_MOUSEMOVE:
        HandleMouseMove(lParam);
        break;

    case WM_MOUSEWHEEL:
        HandleMouseWheel(wParam);
        break;
    }
    return false; // 보통 0 반환(핸들드)해도 되지만, 기본 처리가 필요 없으면 true/0 선택
}

bool UInputManager::IsKeyDown(int keyCode) const
{
    if (keyCode < 0 || keyCode >= 256)
        return false;

    return keyStates[keyCode];
}

bool UInputManager::IsKeyPressed(int keyCode) const
{
    if (keyCode < 0 || keyCode >= 256)
        return false;

    // Key is pressed this frame but was not pressed last frame
    return keyStates[keyCode] && !prevKeyStates[keyCode];
}

bool UInputManager::IsKeyReleased(int keyCode) const
{
    if (keyCode < 0 || keyCode >= 256)
        return false;

    // Key was pressed last frame but is not pressed this frame
    return !keyStates[keyCode] && prevKeyStates[keyCode];
}

bool UInputManager::IsMouseButtonDown(int button) const
{
    if (button < 0 || button >= 3)
        return false;

    return mouseButtons[button];
}

bool UInputManager::IsMouseButtonPressed(int button) const
{
    if (button < 0 || button >= 3)
        return false;

    // Button is pressed this frame but was not pressed last frame
    return mouseButtons[button] && !prevMouseButtons[button];
}

bool UInputManager::IsMouseButtonReleased(int button) const
{
    if (button < 0 || button >= 3)
        return false;

    // Button was pressed last frame but is not pressed this frame
    return !mouseButtons[button] && prevMouseButtons[button];
}

void UInputManager::ResetStates()
{
    // Clear all input states
    memset(keyStates, 0, sizeof(keyStates));
    memset(prevKeyStates, 0, sizeof(prevKeyStates));
    memset(mouseButtons, 0, sizeof(mouseButtons));
    memset(prevMouseButtons, 0, sizeof(prevMouseButtons));

    mouseX = mouseY = 0;
}

void UInputManager::HandleKeyDown(WPARAM wParam)
{
    if (wParam < 256)
    {
        keyStates[wParam] = true;
    }
}

void UInputManager::HandleKeyUp(WPARAM wParam)
{
    if (wParam < 256)
    {
        keyStates[wParam] = false;
    }
}

void UInputManager::HandleMouseMove(LPARAM lParam)
{
    int newMouseX = LOWORD(lParam);
    int newMouseY = HIWORD(lParam);

    // Update current mouse position
    mouseX = newMouseX;
    mouseY = newMouseY;
}

void UInputManager::HandleMouseButton(UINT message, WPARAM wParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN:
        mouseButtons[0] = true;
        break;
    case WM_LBUTTONUP:
        mouseButtons[0] = false;
        break;

    case WM_RBUTTONDOWN:
        mouseButtons[1] = true;
        break;
    case WM_RBUTTONUP:
        mouseButtons[1] = false;
        break;

    case WM_MBUTTONDOWN:
        mouseButtons[2] = true;
        break;
    case WM_MBUTTONUP:
        mouseButtons[2] = false;
        break;
    }
}

void UInputManager::HandleMouseWheel(WPARAM wParam)
{
    // Get wheel delta (positive = forward, negative = backward)
    int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
}