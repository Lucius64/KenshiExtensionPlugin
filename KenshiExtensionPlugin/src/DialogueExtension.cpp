#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreStringConverter.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/GameData.h>
#include <kenshi/Faction.h>
#include <kenshi/Dialogue.h>
#include <kenshi/Character.h>
#include <kenshi/FactionRelations.h>
#include <kenshi/Platoon.h>
#include <kenshi/WorldEventStateQuery.h>
#include <kenshi/RaceData.h>
#include <kenshi/SensoryData.h>
#include <kenshi/Inventory.h>
#include <extern/StateBroadcastData.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <DialogueExtension.h>

namespace
{
	void (*FactionRelations_save_orig)(FactionRelations*, GameData*);
	void FactionRelations_save_hook(FactionRelations* self, GameData* factionsList)
	{
		if (KEP::settings._saveReputation)
			factionsList->fdata["reputation"] = self->globalReputationForBadassery;
		FactionRelations_save_orig(self, factionsList);
	}

	void (*FactionRelations_load_orig)(FactionRelations*, GameData*, bool);
	void FactionRelations_load_hook(FactionRelations* self, GameData* gamestate_faction, bool playerOnly)
	{
		FactionRelations_load_orig(self, gamestate_faction, playerOnly);
		if (KEP::settings._saveReputation)
			self->globalReputationForBadassery = gamestate_faction->fdata["reputation"];
	}

	class DetourDialogue : public Dialogue
	{
	public:
		std::string Dialogue_getWordSwap_hook(const std::string& key, Character* _target, bool _swapMeYou, DialogLineData* _line);
	};

	std::string (DetourDialogue::* Dialogue_getWordSwap_orig)(const std::string&, Character*, bool, DialogLineData*);
	std::string DetourDialogue::Dialogue_getWordSwap_hook(const std::string& key, Character* _target, bool _swapMeYou, DialogLineData* _line)
	{
		if (key == "CLR_MAIN")
			return *KEP::externalGlobals->_MainColorCode;
		else if (key == "CLR_SECONDARY")
			return *KEP::externalGlobals->_SecondaryColorCode;
		else if (key == "CLR_TITLE")
			return *KEP::externalGlobals->_TitleColorCode;
		else if (key == "CLR_BAD")
			return *KEP::externalGlobals->_BadColorCode;
		else if (key == "CLR_BAD_BRIGHT")
			return *KEP::externalGlobals->_BadBrightColorCode;
		else if (key == "CLR_GOOD")
			return *KEP::externalGlobals->_GoodColorCode;
		else if (key == "CLR_GOOD_BRIGHT")
			return *KEP::externalGlobals->_GoodBrightColorCode;
		else if (key == "CLR_GREYED")
			return *KEP::externalGlobals->_GreyedColorCode;
		else if (key == "CLR_GREYED_BRIGHT")
			return *KEP::externalGlobals->_GreyedBrightColorCode;
		else if (key == "CLR_SPECIAL")
			return *KEP::externalGlobals->_SpecialColorCode;
		else
		{
			if (_target != nullptr && this->me == _target)
			{
				auto actualTarget = this->conversationMaster.getCharacter();
				if (actualTarget != nullptr && this->me != actualTarget)
					return (this->*Dialogue_getWordSwap_orig)(key, actualTarget, _swapMeYou, _line);
			}
			return (this->*Dialogue_getWordSwap_orig)(key, _target, _swapMeYou, _line);
		}
	}
}

void KEP::DialogueExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&FactionRelations::_NV_save), &FactionRelations_save_hook, &FactionRelations_save_orig))
		ErrorLog("[FactionRelations::save] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&FactionRelations::_NV_load), &FactionRelations_load_hook, &FactionRelations_load_orig))
		ErrorLog("[FactionRelations::load] could not install hook!");

	if (KEP::settings._dialogueExtension)
	{
		auto pfuncTarget = &DetourDialogue::Dialogue_getWordSwap_hook;
		auto pfuncOrig = &Dialogue_getWordSwap_orig;
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Dialogue::getWordSwap), *(void**)&pfuncTarget, *(void***)&pfuncOrig))
			ErrorLog("[Dialogue::getWordSwap] could not install hook!");
	}
}
