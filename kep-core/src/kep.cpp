#include "pch.h"
#include <kep.h>
#include <kep/patch_system.h>

#include <kenshi/Kenshi.h>

namespace
{
	KEP::VersionInfo KenshiLibVersion;

	void setKenshiLibVersion()
	{
		auto hKenshiLib = GetModuleHandleA("KenshiLib.dll");

		FARPROC procAddr = nullptr;
		if (GetProcAddress(hKenshiLib, "?gui@@3PEAVForgottenGUI@@EA") != nullptr)
		{
			KenshiLibVersion = KEP::VersionInfo(0, 3, 0);
		}
		else if (GetProcAddress(hKenshiLib, "??0DataPanelLine_KeyConfig@@QEAA@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@0H@Z") != nullptr)
		{
			KenshiLibVersion = KEP::VersionInfo(0, 2, 3);
		}
		else if (GetProcAddress(hKenshiLib, "??0NavMesh@@QEAA@XZ") != nullptr)
		{
			KenshiLibVersion = KEP::VersionInfo(0, 2, 2);
		}
		else if (GetProcAddress(hKenshiLib, "??0CraftingInventoryLayout@@QEAA@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@HH@Z") != nullptr)
		{
			KenshiLibVersion = KEP::VersionInfo(0, 2, 1);
		}
		else if (GetProcAddress(hKenshiLib, "??0ForgottenGUI@@QEAA@XZ") != nullptr)
		{
			KenshiLibVersion = KEP::VersionInfo(0, 2, 0);
		}
		else if (GetProcAddress(hKenshiLib, "??0AppearanceBase@@QEAA@PEAVAnimationClass@@PEAVGameDataCopyStandalone@@PEAVCharacter@@PEAVGameData@@@Z") != nullptr)
		{
			KenshiLibVersion = KEP::VersionInfo(0, 1, 5);
		}
		else if (GetProcAddress(hKenshiLib, "??0TownBase@@QEAA@PEAVGameData@@@Z") != nullptr)
		{
			KenshiLibVersion = KEP::VersionInfo(0, 1, 4);
		}
		else if (GetProcAddress(hKenshiLib, "??0GameplayOptions@@QEAA@XZ") != nullptr)
		{
			KenshiLibVersion = KEP::VersionInfo(0, 1, 3);
		}
		else if (GetProcAddress(hKenshiLib, "?getInstance@ResourceLoader@@SAPEAV1@XZ") != nullptr)
		{
			KenshiLibVersion = KEP::VersionInfo(0, 1, 2);
		}
		else if (GetProcAddress(hKenshiLib, "??0ParticlePool@@QEAA@HM@Z") != nullptr)
		{
			KenshiLibVersion = KEP::VersionInfo(0, 1, 1);
		}
		else
		{
			KenshiLibVersion = KEP::VersionInfo(0, 1, 0);
		}
	}
}

const KEP::VersionInfo& KEP::getKenshiLibVersion()
{
	return KenshiLibVersion;
}

void KEP::initialize()
{
	auto versionInfo = KenshiLib::GetKenshiVersion();
	auto platform = versionInfo.GetPlatform();
	auto version = versionInfo.GetVersion();
	auto baseAddr = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));

	setKenshiLibVersion();
	functions->init(platform, version, baseAddr);
	TranslationUtility::init(platform, version, baseAddr);
	ConfigManager::init(platform, version, baseAddr);
	KEP::GUIColor::init(platform, version, baseAddr);
	KEP::PatchSystem::init();
}

namespace
{
	std::string* _MainColorCode = nullptr;
	std::string* _SecondaryColorCode = nullptr;
	std::string* _TitleColorCode = nullptr;
	std::string* _BadColorCode = nullptr;
	std::string* _BadBrightColorCode = nullptr;
	std::string* _GoodColorCode = nullptr;
	std::string* _GoodBrightColorCode = nullptr;
	std::string* _GreyedColorCode = nullptr;
	std::string* _GreyedBrightColorCode = nullptr;
	std::string* _SpecialColorCode = nullptr;
	MyGUI::Colour* _NameTagSelectedColor = nullptr;
	MyGUI::Colour* _NameTagUnselectedColor = nullptr;
	MyGUI::Colour* _NameTagStealthHiddenColor = nullptr;
	MyGUI::Colour* _NameTagStealthAlmostSeenColor = nullptr;
	MyGUI::Colour* _NameTagStealthSeenColor = nullptr;
	MyGUI::Colour* _DialogueBubbleTextColor = nullptr;
}

void KEP::GUIColor::init(unsigned int platform, const std::string& version, uintptr_t baseAddr)
{
	if (platform == 1)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&_MainColorCode = baseAddr + 0x01f48238;
			*(uintptr_t*)&_SecondaryColorCode = baseAddr + 0x01f48260;
			*(uintptr_t*)&_TitleColorCode = baseAddr + 0x01f48288;
			*(uintptr_t*)&_BadColorCode = baseAddr + 0x01f482b0;
			*(uintptr_t*)&_BadBrightColorCode = baseAddr + 0x01f482d8;
			*(uintptr_t*)&_GoodColorCode = baseAddr + 0x01f48300;
			*(uintptr_t*)&_GoodBrightColorCode = baseAddr + 0x01f48328;
			*(uintptr_t*)&_GreyedColorCode = baseAddr + 0x01f48350;
			*(uintptr_t*)&_GreyedBrightColorCode = baseAddr + 0x01f48378;
			*(uintptr_t*)&_SpecialColorCode = baseAddr + 0x01f483a0;
			*(uintptr_t*)&_NameTagSelectedColor = baseAddr + 0x02131aa0;
			*(uintptr_t*)&_NameTagUnselectedColor = baseAddr + 0x02131ab0;
			*(uintptr_t*)&_NameTagStealthHiddenColor = baseAddr + 0x02131ac0;
			*(uintptr_t*)&_NameTagStealthAlmostSeenColor = baseAddr + 0x02131ad0;
			*(uintptr_t*)&_NameTagStealthSeenColor = baseAddr + 0x02131ae0;
			*(uintptr_t*)&_DialogueBubbleTextColor = baseAddr + 0x02131c00;
		}
		else if (version == "1.0.68")
		{
			*(uintptr_t*)&_MainColorCode = baseAddr + 0x01f49248;
			*(uintptr_t*)&_SecondaryColorCode = baseAddr + 0x01f49270;
			*(uintptr_t*)&_TitleColorCode = baseAddr + 0x01f49298;
			*(uintptr_t*)&_BadColorCode = baseAddr + 0x01f492c0;
			*(uintptr_t*)&_BadBrightColorCode = baseAddr + 0x01f492e8;
			*(uintptr_t*)&_GoodColorCode = baseAddr + 0x01f49310;
			*(uintptr_t*)&_GoodBrightColorCode = baseAddr + 0x01f49338;
			*(uintptr_t*)&_GreyedColorCode = baseAddr + 0x01f49360;
			*(uintptr_t*)&_GreyedBrightColorCode = baseAddr + 0x01f49388;
			*(uintptr_t*)&_SpecialColorCode = baseAddr + 0x01f493b0;
			*(uintptr_t*)&_NameTagSelectedColor = baseAddr + 0x02132ae0;
			*(uintptr_t*)&_NameTagUnselectedColor = baseAddr + 0x02132af0;
			*(uintptr_t*)&_NameTagStealthHiddenColor = baseAddr + 0x02132b00;
			*(uintptr_t*)&_NameTagStealthAlmostSeenColor = baseAddr + 0x02132b10;
			*(uintptr_t*)&_NameTagStealthSeenColor = baseAddr + 0x02132b20;
			*(uintptr_t*)&_DialogueBubbleTextColor = baseAddr + 0x02132c50;
		}
	}
	else if (platform == 0)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&_MainColorCode = baseAddr + 0x01f46248;
			*(uintptr_t*)&_SecondaryColorCode = baseAddr + 0x01f46270;
			*(uintptr_t*)&_TitleColorCode = baseAddr + 0x01f46298;
			*(uintptr_t*)&_BadColorCode = baseAddr + 0x01f462c0;
			*(uintptr_t*)&_BadBrightColorCode = baseAddr + 0x01f462e8;
			*(uintptr_t*)&_GoodColorCode = baseAddr + 0x01f46310;
			*(uintptr_t*)&_GoodBrightColorCode = baseAddr + 0x01f46338;
			*(uintptr_t*)&_GreyedColorCode = baseAddr + 0x01f46360;
			*(uintptr_t*)&_GreyedBrightColorCode = baseAddr + 0x01f46388;
			*(uintptr_t*)&_SpecialColorCode = baseAddr + 0x01f463b0;
			*(uintptr_t*)&_NameTagSelectedColor = baseAddr + 0x0212fa10;
			*(uintptr_t*)&_NameTagUnselectedColor = baseAddr + 0x0212fa20;
			*(uintptr_t*)&_NameTagStealthHiddenColor = baseAddr + 0x0212fa30;
			*(uintptr_t*)&_NameTagStealthAlmostSeenColor = baseAddr + 0x0212fa40;
			*(uintptr_t*)&_NameTagStealthSeenColor = baseAddr + 0x0212fa50;
			*(uintptr_t*)&_DialogueBubbleTextColor = baseAddr + 0x0212fb70;
		}
		else if (version == "1.0.68")
		{
			*(uintptr_t*)&_MainColorCode = baseAddr + 0x01f48248;
			*(uintptr_t*)&_SecondaryColorCode = baseAddr + 0x01f48270;
			*(uintptr_t*)&_TitleColorCode = baseAddr + 0x01f48298;
			*(uintptr_t*)&_BadColorCode = baseAddr + 0x01f482c0;
			*(uintptr_t*)&_BadBrightColorCode = baseAddr + 0x01f482e8;
			*(uintptr_t*)&_GoodColorCode = baseAddr + 0x01f48310;
			*(uintptr_t*)&_GoodBrightColorCode = baseAddr + 0x01f48338;
			*(uintptr_t*)&_GreyedColorCode = baseAddr + 0x01f48360;
			*(uintptr_t*)&_GreyedBrightColorCode = baseAddr + 0x01f48388;
			*(uintptr_t*)&_SpecialColorCode = baseAddr + 0x01f483b0;
			*(uintptr_t*)&_NameTagSelectedColor = baseAddr + 0x02131a10;
			*(uintptr_t*)&_NameTagUnselectedColor = baseAddr + 0x02131a20;
			*(uintptr_t*)&_NameTagStealthHiddenColor = baseAddr + 0x02131a30;
			*(uintptr_t*)&_NameTagStealthAlmostSeenColor = baseAddr + 0x02131a40;
			*(uintptr_t*)&_NameTagStealthSeenColor = baseAddr + 0x02131a50;
			*(uintptr_t*)&_DialogueBubbleTextColor = baseAddr + 0x02131b80;
		}
	}
}

const std::string& KEP::GUIColor::getMain() { return *_MainColorCode; }
const std::string& KEP::GUIColor::getSecondary() { return *_SecondaryColorCode; }
const std::string& KEP::GUIColor::getTitle() { return *_TitleColorCode; }
const std::string& KEP::GUIColor::getBad() { return *_BadColorCode; }
const std::string& KEP::GUIColor::getBadBright() { return *_BadBrightColorCode; }
const std::string& KEP::GUIColor::getGood() { return *_GoodColorCode; }
const std::string& KEP::GUIColor::getGooddBright() { return *_GoodBrightColorCode; }
const std::string& KEP::GUIColor::getGreyed() { return *_GreyedColorCode; }
const std::string& KEP::GUIColor::getGreyedBright() { return *_GreyedBrightColorCode; }
const std::string& KEP::GUIColor::getSpecial() { return *_SpecialColorCode; }
const MyGUI::Colour& KEP::GUIColor::getNameTagSelected() { return *_NameTagSelectedColor; }
const MyGUI::Colour& KEP::GUIColor::getNameTagUnselected() { return *_NameTagUnselectedColor; }
const MyGUI::Colour& KEP::GUIColor::getNameTagStealthHidden() { return *_NameTagStealthHiddenColor; }
const MyGUI::Colour& KEP::GUIColor::getNameTagStealthAlmostSeen() { return *_NameTagStealthAlmostSeenColor; }
const MyGUI::Colour& KEP::GUIColor::getTagStealthSeen() { return *_NameTagStealthSeenColor; }
const MyGUI::Colour& KEP::GUIColor::geDialogueBubbleText() { return *_DialogueBubbleTextColor; }
