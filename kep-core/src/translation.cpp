#include "pch.h"
#include <boost/filesystem.hpp>

#include <ogre/OgreConfigFile.h>

#include <kep/translation.h>
#include <kep/utility.h>
#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GlobalConstants.h>

namespace fs = boost::filesystem;

class LocaleInfo : private Ogre::GeneralAllocatedObject
{
public:
	bool _0x0;
	std::string localeCode;
	std::string displayName;
	std::string language;
	bool isMod;
};

class LocaleInfoManager
{
public:
	ogre_unordered_map<std::string, LocaleInfo*>::type localeCodeMap;
	Ogre::FastArray<LocaleInfo> langList;
	Ogre::FastArray<LocaleInfo*> _0x58;
	LocaleInfo* newLocale;
	LocaleInfo* useLocale;
	std::wstring path;
};

namespace
{
	std::locale* KenshiLocale = nullptr;
	std::string (boost::locale::message::* message_str)() const = nullptr; // boost::locale::basic_message<char>::str
	std::string (boost::locale::format::* format_str)(const std::locale&) const = nullptr; // boost::locale::basic_format<char>::str
	
	std::string lang = "en_GB";

	void setLocale(const std::string& language)
	{
		auto& modulePath = KEP::getPluginPath();
		std::string messagesPath = modulePath + "\\locale";
		auto domain = "kep_main";
		KEP::TranslationUtility::getMessagesPath(domain, language, messagesPath);

		boost::locale::generator gen;
		gen.add_messages_path(messagesPath);
		gen.add_messages_domain(domain);
		std::locale::global(gen.generate(std::locale(), language + ".UTF-8"));
	}

	LocaleInfoManager* (*LocaleInfoManager_getSingleton)();
	bool (*setModTranslation)(LocaleInfoManager*, ModInfo*);

	bool (*setModTranslation_orig)(LocaleInfoManager*, ModInfo*);
	bool setModTranslation_hook(LocaleInfoManager* self, ModInfo* mod)
	{
		bool result = setModTranslation_orig(self, mod);
		if (result)
		{
			lang = self->useLocale->localeCode;
		}
		return result;
	}

	void (*GlobalConstants_setup_orig)(GlobalConstants*, GameData*);
	void GlobalConstants_setup_hook(GlobalConstants* self, GameData* data)
	{
		GlobalConstants_setup_orig(self, data);
		setLocale(lang);
	}
}

void KEP::TranslationUtility::init(unsigned int platform, const std::string& version, uintptr_t baseAddr)
{
	if (platform == 1)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&KenshiLocale = baseAddr + 0x2131eb8;
			*(uintptr_t*)&message_str = baseAddr + 0xa9580;
			*(uintptr_t*)&format_str = baseAddr + 0xa9730;
			*(uintptr_t*)&LocaleInfoManager_getSingleton = baseAddr + 0x120560;
			*(uintptr_t*)&setModTranslation = baseAddr + 0x174d10;
		}
		else if (version == "1.0.68")
		{
			*(uintptr_t*)&KenshiLocale = baseAddr + 0x2132f08;
			*(uintptr_t*)&message_str = baseAddr + 0xa95c0;
			*(uintptr_t*)&format_str = baseAddr + 0xa9770;
			*(uintptr_t*)&LocaleInfoManager_getSingleton = baseAddr + 0x1205a0;
			*(uintptr_t*)&setModTranslation = baseAddr + 0x174d50;
		}
	}
	else if (platform == 0)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&KenshiLocale = baseAddr + 0x212fe28;
			*(uintptr_t*)&message_str = baseAddr + 0xa9580;
			*(uintptr_t*)&format_str = baseAddr + 0xa9730;
			*(uintptr_t*)&LocaleInfoManager_getSingleton = baseAddr + 0x120560;
			*(uintptr_t*)&setModTranslation = baseAddr + 0x174e00;
		}
		else if (version == "1.0.68")
		{
			*(uintptr_t*)&KenshiLocale = baseAddr + 0x2131e38;
			*(uintptr_t*)&message_str = baseAddr + 0xa95a0;
			*(uintptr_t*)&format_str = baseAddr + 0xa9750;
			*(uintptr_t*)&LocaleInfoManager_getSingleton = baseAddr + 0x120580;
			*(uintptr_t*)&setModTranslation = baseAddr + 0x174c50;
		}
	}

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(setModTranslation, &setModTranslation_hook, &setModTranslation_orig))
		ErrorLog("[setModTranslation] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&GlobalConstants::setup), &GlobalConstants_setup_hook, &GlobalConstants_setup_orig))
		ErrorLog("[GlobalConstants::setup] could not install hook!");

	auto localeMgr = LocaleInfoManager_getSingleton();
	if (localeMgr != nullptr && localeMgr->useLocale != nullptr)
	{
		lang = localeMgr->useLocale->localeCode;
	}
	else
	{
		Ogre::ConfigFile config;
		config.load("settings.cfg");
		lang = config.getSetting("language");
	}
}

const std::string& KEP::TranslationUtility::getLanguage()
{
	return lang;
}

std::string KEP::TranslationUtility::gettext_main(const char* id)
{
	return (boost::locale::translate(id).*message_str)();
}

std::string KEP::TranslationUtility::gettext_main(const boost::locale::message& msg)
{
	return (msg.*message_str)();
}

std::string KEP::TranslationUtility::format_main(const boost::locale::format& fmt)
{
	return (fmt.*format_str)(*KenshiLocale);
}

std::string KEP::TranslationUtility::dgettext(const char* domain, const char* id)
{
	return boost::locale::dgettext(domain, id);
}

std::string KEP::TranslationUtility::gettext(const char* id)
{
	return boost::locale::gettext(id);
}

std::string KEP::TranslationUtility::str(const std::string& domain, const boost::locale::message& msg)
{
	return msg.str(domain);
}

std::string KEP::TranslationUtility::format(const boost::locale::format& fmt)
{
	return fmt.str();
}

const boost::locale::collator<char>& KEP::TranslationUtility::getCollator()
{
	return std::use_facet<boost::locale::collator<char>>(std::locale());
}

void KEP::TranslationUtility::getMessagesPath(const std::string& domain, const std::string& language, std::string& filepath)
{
	fs::path search(language);
	search /= "LC_MESSAGES";
	search /= domain + ".mo";

	for (uint32_t i = ou->activeMods.size(); i != 0; --i)
	{
		fs::path modPath(ou->activeMods[i - 1]->path);
		modPath /= "locale";
		if (fs::exists(modPath / search))
		{
			filepath.assign(modPath.string());
			break;
		}
	}
}
