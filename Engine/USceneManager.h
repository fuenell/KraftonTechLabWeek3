#pragma once
class UScene;
class UApplication;

class USceneManager 
{
private:
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

