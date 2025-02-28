#pragma once


class Engine;

static constexpr char* myLocScriptPath = "ScriptData/";

namespace Settings {
extern void LoadSettings(const std::string& aProjectName, Engine& aEngine);
	namespace Window {
		extern std::string title;
	}
	
	extern std::string ResolveGameScriptPath(const std::string& aScriptPath);
	extern std::string ResolveAssetPath(const std::string& anAsset);
	extern std::wstring ResolveAssetPathW(const std::string& anAsset);
	extern std::wstring ResolveAssetPathW(const std::wstring& anAsset);

	extern std::string GameSettings(const std::string& aKey);
	extern std::wstring GameSettingsW(const std::string& aKey);
	extern std::wstring GameSettingsW(const std::wstring& aKey);
}
