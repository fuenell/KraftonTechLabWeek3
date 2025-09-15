#pragma once
#include "stdafx.h"
#include "UCamera.h"
#include "ULineBatcherManager.h"

class ConfigManager
{
private:
	ConfigManager() = default;
public:
	static ConfigManager& Instance();
	void RegisterConfigTargets(ULineBatcherManager* LineBatcherManager);
	void SaveConfig();
	void LoadConfig();

	
private:
	ULineBatcherManager* LineBatcherManager;

	static const inline FString ConfigFileName = "editor.ini";
};