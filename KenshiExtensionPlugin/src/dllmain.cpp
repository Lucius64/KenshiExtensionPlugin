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
#include <PluginMenu.h>
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


namespace fs = boost::filesystem;


__declspec(dllexport) void startPlugin()
{
	auto versionInfo = KenshiLib::GetKenshiVersion();
	auto platform = versionInfo.GetPlatform();
	auto version = versionInfo.GetVersion();

	if (platform == KenshiLib::BinaryVersion::UNKNOWN || version != "1.0.65")
	{
		ErrorLog("KenshiExtensionPlugin: error!");
		return;
	}

	DebugLog("KenshiExtensionPlugin: installing");

	auto baseAddr = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
	
	KEP::externalGlobals->init(platform, version, baseAddr);
	KEP::externalFunctions->init(platform, version, baseAddr);

	KEP::settings.init(KEP::myDirectory + "\\kep_settings.json");
	KEP::settings.loadSettings();
	KEP::settings.saveSettings();

	Ogre::ConfigFile config;
	config.load("settings.cfg");
	std::string language = config.getSetting("language");
	boost::locale::generator gen;
	gen.add_messages_path(KEP::myDirectory + "\\locale");
	gen.add_messages_domain("kep_main");
	std::locale::global(gen.generate(std::locale(), language + ".UTF-8"));

	KEP::PluginMenu::init();
	KEP::AppearanceFix::init();
	KEP::UniqueCharacterFix::init();
	KEP::StatsFix::init();
	KEP::MedicalFix::init();
	KEP::ItemFix::init();
	KEP::WorldStateFix::init();
	KEP::NavMeshFix::init();
	KEP::TownOverrideFix::init();
	KEP::DistantTownFix::init();
	KEP::ResearchFix::init();
	KEP::GUIFix::init();
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
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		char pluginPath[_MAX_PATH];
		GetModuleFileNameA(hModule, pluginPath, _MAX_PATH);
		fs::path s = pluginPath;
		KEP::myDirectory = s.branch_path().string();
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
