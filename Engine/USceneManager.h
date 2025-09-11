#pragma once
#include "UEngineSubsystem.h"

class UScene;
class Application;

class USceneManager : UEngineSubsystem
{
    DECLARE_UCLASS(USceneManager, UEngineSubsystem)
private:
    Application* application;
    UScene* currentScene = nullptr;
public:
    ~USceneManager() override;
    bool Initialize(Application* _application);

    UScene* GetScene();
    void SetScene(UScene* scene);

    void RequestExit();
    void LoadScene(const FString& path = "");
    void SaveScene(const FString& path = "");
};

