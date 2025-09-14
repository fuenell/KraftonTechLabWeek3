#include "stdafx.h"
#include "USceneManager.h"
#include "UScene.h"
#include "Application.h"


IMPLEMENT_UCLASS(USceneManager, UEngineSubsystem)
USceneManager::~USceneManager()
{
	if (CurrentScene)
	{
		delete CurrentScene;
	}
}

bool USceneManager::Initialize(Application* InApplication)
{
	App = InApplication;
	CurrentScene = CreateDefaultScene();
	CurrentScene->Initialize(
		&App->GetRenderer(),
		&App->GetMeshManager(),
		&App->GetInputManager());
	return true;
}

void USceneManager::RequestExit()
{
	if (App)
	{
		App->RequestExit();
	}
}

UScene* USceneManager::GetScene()
{
	return CurrentScene;
}

void USceneManager::SetScene(UScene* Scene)
{
	if (Scene == nullptr)
	{
		return;
	}

	if (CurrentScene != nullptr)
	{
		delete CurrentScene;
	}

	CurrentScene = Scene;

	CurrentScene->Initialize(
		&App->GetRenderer(),
		&App->GetMeshManager(),
		&App->GetInputManager());

	App->OnSceneChange();
}

UScene* USceneManager::CreateDefaultScene()
{
	UScene* NewScene = LoadScene(SavePath + FString(SceneName) + SceneExtension);
	if (NewScene == nullptr)
	{
		NewScene = new UScene();
	}
	return NewScene;
}

UScene* USceneManager::LoadScene(const FString& Path)
{
	std::ifstream File(Path);
	if (!File)
	{
		UE_LOG("The scene does not exist.");
		return nullptr;
	}

	std::stringstream Buffer;
	Buffer << File.rdbuf();

	json::JSON SceneData = json::JSON::Load(Buffer.str());
	UScene* NewScene = UScene::Create(SceneData);
	return NewScene;
}

void USceneManager::SaveScene(const FString& Path)
{
	std::filesystem::path fsPath(Path);

	if (std::filesystem::exists(fsPath))
	{
		std::ifstream File(Path);
		if (File)
		{
			std::stringstream Buffer;
			Buffer << File.rdbuf();
			json::JSON SceneData = json::JSON::Load(Buffer.str());

			CurrentScene->SetVersion(SceneData["Version"].ToInt() + 1);
		}
	}

	json::JSON SceneData = CurrentScene->Serialize();

	std::ofstream File(Path);

	if (!File)
	{
		// Log error: failed to open file
		return;
	}

	File << SceneData.dump();
}

