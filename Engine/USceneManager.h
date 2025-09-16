#pragma once
#include "UEngineSubsystem.h"

class UScene;
class Application;

class USceneManager : UEngineSubsystem
{
	DECLARE_UCLASS(USceneManager, UEngineSubsystem)

public:
	static USceneManager& GetInstance()
	{
		static USceneManager Instance; // 스택(static storage) 싱글톤
		return Instance;
	}

	USceneManager()
	{

	}
	~USceneManager() override;
	bool Initialize(Application* InApplication);

	UScene* GetScene();
	void SetScene(UScene* Scene);

	UScene* CreateDefaultScene();

	void RequestExit();
	UScene* LoadScene(const FString& Path = "");
	void SaveScene(const FString& Path = "");

public:
	char SceneName[256] = "Default";
	const FString SavePath = "./data/";
	const FString SceneExtension = ".Scene";

private:
	Application* App;
	UScene* CurrentScene = nullptr;
};
