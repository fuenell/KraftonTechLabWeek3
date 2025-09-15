#include "stdafx.h"
#include "ConfigManager.h"

ConfigManager& ConfigManager::Instance()
{
	static ConfigManager Instance;
	return Instance;
}

void ConfigManager::RegisterConfigTargets(ULineBatcherManager* LineBatcherManager)
{
	this->LineBatcherManager = LineBatcherManager;
}

void ConfigManager::SaveConfig()
{
	std::ofstream OutFile(ConfigFileName);

	// 실패하면 다음 로드 때 각 객체의 기본값을 사용한다.
	if (!OutFile.is_open()) return;

	// 카메라 관련 설정 저장
	OutFile << "camera rotation sensitivity=" << UCamera::GetRotationSensitivity() << "\n";
	OutFile << "camera translation sensitivity=" << UCamera::GetTranslationSensitivity() << "\n";

	// 그리드 관련 설정 저장
	OutFile << "grid space=" << LineBatcherManager->GetGridSpace();
}

void ConfigManager::LoadConfig()
{
	std::ifstream InFIle(ConfigFileName);

	// 파일이 없으면 각 객체의 기본값을 사용한다.
	if (!InFIle.is_open()) return;

	std::string Line;
	while (std::getline(InFIle, Line)) {
		if (Line.empty() || Line[0] == '[') continue; // 섹션 스킵

		std::istringstream Iss(Line);
		std::string Key;
		if (std::getline(Iss, Key, '=')) {
			std::string Value;
			if (std::getline(Iss, Value)) {
				// 카메라 관련 설정 로드
				if (Key == "camera rotation sensitivity")
					UCamera::SetRotationSensitivity(std::stof(Value));
				else if (Key == "camera translation sensitivity")
					UCamera::SetTranslationSensitivity(std::stof(Value));
				// 그리드 관련 설정 로드
				else if (Key == "grid space")
					LineBatcherManager->SetGridSpace(std::stof(Value));
			}
		}
	}
}