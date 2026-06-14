#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <boost/filesystem.hpp>
#include <boost/locale.hpp>

#include <ogre/OgreConfigFile.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <bugfix/AppearanceFix.h>
#include <bugfix/UniqueCharacterFix.h>
#include <bugfix/StatsFix.h>
#include <bugfix/ItemFix.h>
#include <bugfix/MedicalFix.h>
#include <bugfix/MiscFix.h>
#include <WarCampaignExtension.h>
#include <CharacterExtension.h>
#include <ItemFurnaceExtension.h>
#include <RaceLimiterExtension.h>
#include <ScytheFileExtension.h>
#include <AppearanceExtension.h>
#include <AnimationExtension.h>
#include <DialogueExtension.h>
#include <AnimalExtension.h>
#include <CraftingExtension.h>
#include <ItemExtension.h>
#include <kep.h>

namespace fs = boost::filesystem;

namespace
{
	std::string myDirectory;
}

__declspec(dllexport) void startPlugin()
{
	DebugLog("kep 0.15.4");

	auto& libVersion = KEP::getKenshiLibVersion();
	if (libVersion < KEP::VersionInfo(0, 3, 0))
	{
		ErrorLog("Please install KenshiLib version 0.3.0 or later.");
		return;
	}

	auto versionInfo = KenshiLib::GetKenshiVersion();
	auto platform = versionInfo.GetPlatform();
	auto version = versionInfo.GetVersion();

	if (platform == KenshiLib::BinaryVersion::UNKNOWN || (version != "1.0.65" && version != "1.0.68"))
	{
		ErrorLog("error!");
		return;
	}

	fs::path modulePath = KEP::getPluginPath();
	if (!fs::exists(modulePath / "LICENSE") || !fs::exists(modulePath / "NOTICE.md"))
	{
		ErrorLog("License file not found.");
		return;
	}

	auto baseAddr = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
	
	KEP::externalGlobals->init(platform, version, baseAddr);
	KEP::externalFunctions->init(platform, version, baseAddr);

	KEP::settings.init(myDirectory + "\\kep_settings.json");
	KEP::settings.loadSettings();
	KEP::settings.saveSettings();
	auto& confMgr = KEP::ConfigManager::getSingleton();
	confMgr.addItem(&KEP::settings);

	KEP::AppearanceFix::init();
	KEP::UniqueCharacterFix::init();
	KEP::StatsFix::init();
	KEP::MedicalFix::init();
	KEP::ItemFix::init();
	KEP::MiscFix::init();
	KEP::WarCampaignExtension::init();
	KEP::CharacterExtension::init();
	KEP::ItemFurnaceExtension::init();
	KEP::RaceLimiterExtension::init();
	KEP::ScytheFileExtension::init();
	KEP::AppearanceExtension::init();
	KEP::AnimationExtension::init();
	KEP::DialogueExtension::init();
	KEP::AnimalExtension::init();
	KEP::CraftingExtension::init();
	KEP::ItemExtension::init();
	DebugLog("Installed");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		DWORD bufSize = _MAX_PATH;
		std::vector<TCHAR> buf(bufSize + 1, 0);
		if (GetModuleFileNameW(hModule, buf.data(), bufSize) != 0)
			myDirectory = fs::path(buf.data()).branch_path().string();

		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			bufSize = MAXSHORT;
			buf.resize(static_cast<size_t>(bufSize) + 1);
			if (GetModuleFileNameW(hModule, buf.data(), bufSize) != 0)
				myDirectory = fs::path(buf.data()).branch_path().string();
		}
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
