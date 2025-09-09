#pragma once
#include "UEngineSubsystem.h"

class UScene;
class UApplication;

class USceneManager : UEngineSubsystem
{
private:
    ~USceneManager();
    UApplication* application;
    UScene* currentScene = nullptr;
public:
    bool Initialize(UApplication* _application);

    UScene* GetScene();
    void SetScene(UScene* scene);

    void RequestExit();
    void LoadScene(const std::string& path = "");
    void SaveScene(const std::string& path = "");
};

