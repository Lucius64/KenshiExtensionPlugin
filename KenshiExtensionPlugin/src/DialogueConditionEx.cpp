/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreStringConverter.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/Faction.h>
#include <kenshi/Dialogue.h>
#include <kenshi/Character.h>
#include <kenshi/Platoon.h>
#include <kenshi/WorldEventStateQuery.h>
#include <kenshi/RaceData.h>
#include <kenshi/SensoryData.h>
#include <kenshi/Inventory.h>
#include <kenshi/StateBroadcastData.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/Town.h>
#include <kenshi/SharedKing.h>

#include <extern/AreaBiomeGroup.h>

#include <kep/utility.h>
#include <kep/translation.h>
#include <kep/functions.h>
#include <Settings.h>
#include <EnumExtended.h>
#include <DialogueConditionEx.h>

namespace
{
	class DialogLineDataExtend
	{
	public:
		DialogLineDataExtend(DialogLineData* _line);
		void _initialiseAList(const std::string& listname, lektor<GameData*>& list) const;
		bool checkActualTargetRace(Character* target);
		bool checkActualTargetSubRace(Character* target);
		bool checkTargetCharacter(Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers);
		bool checkActualTargetCharacter(Character* target);
		bool isInArea(Dialogue* dialog);
		bool isInSpecificTown(Dialogue* dialog);
		bool isInReplacementTown(Dialogue* dialog);

		DialogLineData* line;
		lektor<GameData*> isTargetCharacter;
		lektor<GameData*> isActualTargetCharacter;
		lektor<GameData*> isActualTargetRace;
		lektor<GameData*> isActualTargetSubRace;
		lektor<GameData*> inAreaOf;
		lektor<GameData*> inSpecificTownOf;
		lektor<GameData*> inReplacementTownOf;
	};

	DialogLineDataExtend::DialogLineDataExtend(DialogLineData* _line)
		: line(_line)
	{
		_initialiseAList("target character", isTargetCharacter);
		_initialiseAList("target animal", isTargetCharacter);
		_initialiseAList("actual target character", isActualTargetCharacter);
		_initialiseAList("actual target animal", isActualTargetCharacter);
		_initialiseAList("actual target race", isActualTargetRace);
		_initialiseAList("actual target subrace", isActualTargetSubRace);
		_initialiseAList("in area of", inAreaOf);
		_initialiseAList("in specific town of", inSpecificTownOf);
		_initialiseAList("in replacement town of", inReplacementTownOf);

		if (isTargetCharacter.size() != 0)
			line->score += 2;
		if (isActualTargetCharacter.size() != 0)
			line->score += 2;
		if (isActualTargetRace.size() != 0)
			line->score += 2;
		if (isActualTargetSubRace.size() != 0)
			line->score += 2;
		if (inAreaOf.size() != 0)
			line->score += 2;
		if (inSpecificTownOf.size() != 0)
			line->score += 2;
		if (inReplacementTownOf.size() != 0)
			line->score += 2;
	}

	void DialogLineDataExtend::_initialiseAList(const std::string& listname, lektor<GameData*>& list) const
	{
		auto referencelist = this->line->data->getReferenceListIfExists(listname);
		if (referencelist != nullptr)
		{
			for (auto iter = referencelist->begin(); iter != referencelist->end(); ++iter)
			{
				auto data = iter->getPtr(&ou->gamedata);
				if (data != nullptr)
					list.push_back(data);
			}
		}
	}

	bool DialogLineDataExtend::checkActualTargetRace(Character* target)
	{
		if (isActualTargetRace.size() == 0)
			return true;

		if (target == nullptr)
			return false;

		auto race = target->getRace();
		for (auto iter = isActualTargetRace.begin(); iter != isActualTargetRace.end(); ++iter)
		{
			if (race->isRelatedRace(*iter))
				return true;
		}
		return false;
	}

	bool DialogLineDataExtend::checkActualTargetSubRace(Character* target)
	{
		if (isActualTargetSubRace.size() == 0)
			return true;

		if (target == nullptr)
			return false;

		auto race = target->getRace();
		for (auto iter = isActualTargetSubRace.begin(); iter != isActualTargetSubRace.end(); ++iter)
		{
			if (race->data == *iter)
				return true;
		}
		return false;
	}

	bool DialogLineDataExtend::checkTargetCharacter(Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers)
	{
		if (isTargetCharacter.size() == 0)
			return true;

		if (target == nullptr)
			return false;

		auto data = target->getGameData();
		for (auto iter = isTargetCharacter.begin(); iter != isTargetCharacter.end(); ++iter)
		{
			if (data == *iter)
				return true;
		}

		if (ignoreTargetMembers)
			return false;

		for (auto iter = isTargetCharacter.begin(); iter != isTargetCharacter.end(); ++iter)
		{
			auto platoon = target->getPlatoon();
			for (auto squadMemberIter = platoon->things.begin(); squadMemberIter != platoon->things.end(); ++squadMemberIter)
			{
				auto squadMember = static_cast<Character*>(*squadMemberIter);
				if (squadMember->getGameData() == *iter)
				{
					if (squadMember != target)
						if (!dialog->me->getSensoryData()->amIAwareOfThisGuy(squadMember, false))
							continue;

					if (!isWordswap)
						dialog->changeConversationTarget(squadMember);
					return true;
				}
			}
		}
		return false;
	}

	bool DialogLineDataExtend::checkActualTargetCharacter(Character* target)
	{
		if (isActualTargetCharacter.size() == 0)
			return true;

		if (target == nullptr)
			return false;

		auto data = target->getGameData();
		for (auto iter = isActualTargetCharacter.begin(); iter != isActualTargetCharacter.end(); ++iter)
		{
			if (data == *iter)
				return true;
		}
		return false;
	}

	bool DialogLineDataExtend::isInArea(Dialogue* dialog)
	{
		if (inAreaOf.size() == 0)
			return true;

		auto biome = KEP::functions->AreasList_getBiome(shou->areasList, dialog->me->getPosition());
		if (biome == nullptr)
			return false;

		for (auto iter = inAreaOf.begin(); iter != inAreaOf.end(); ++iter)
		{
			if (biome->data == *iter)
				return true;
		}
		return false;
	}

	bool DialogLineDataExtend::isInSpecificTown(Dialogue* dialog)
	{
		if (inSpecificTownOf.size() == 0)
			return true;

		auto town = dialog->me->getCurrentTownLocation();
		if (town == nullptr)
			return false;

		for (auto iter = inSpecificTownOf.begin(); iter != inSpecificTownOf.end(); ++iter)
		{
			if (town->data == *iter)
				return true;
		}
		return false;
	}

	bool DialogLineDataExtend::isInReplacementTown(Dialogue* dialog)
	{
		if (inReplacementTownOf.size() == 0)
			return true;

		auto townBase = dialog->me->getCurrentTownLocation();
		if (townBase == nullptr)
			return false;

		auto town = townBase->isTown();
		if (town == nullptr || town->replacementTown == nullptr)
			return false;

		for (auto iter = inReplacementTownOf.begin(); iter != inReplacementTownOf.end(); ++iter)
		{
			if (town->replacementTown == *iter)
				return true;
		}
		return false;
	}

	ogre_unordered_map<DialogLineData*, DialogLineDataExtend*>::type lineDataExtends;

	void (*DialogLineData__DESTRUCTOR_orig)(DialogLineData* self);
	void DialogLineData__DESTRUCTOR_hook(DialogLineData* self)
	{
		if (KEP::settings._dialogueConditionEx)
		{
			auto lineDataExtend = lineDataExtends[self];
			if (lineDataExtend != nullptr)
				delete lineDataExtend;
		}
		DialogLineData__DESTRUCTOR_orig(self);
	}

	DialogLineData* (*DialogLineData__CONSTRUCTOR_orig)(DialogLineData*, GameData*);
	DialogLineData* DialogLineData__CONSTRUCTOR_hook(DialogLineData* self, GameData* dat)
	{
		DialogLineData__CONSTRUCTOR_orig(self, dat);
		if (KEP::settings._dialogueConditionEx)
		{
			self->_initialiseAList("target subrace", &self->isTargetSubRace_specificallyTheTarget);

			if (self->isCharacter.size() != 0)
				self->score -= 2;
			self->_initialiseAList("is animal", &self->isCharacter);

			lineDataExtends[self] = new DialogLineDataExtend(self);
		}
		return self;
	}

	bool _isForSpecificPackage(DialogLineData* self, Dialogue* dialog)
	{
		for (auto iter = self->_hasPackage.begin(); iter != self->_hasPackage.end(); ++iter)
		{
			if (dialog->pacakgesIHave.count(*iter) != 0)
				return true;
		}
		return false;
	}

	bool _checkTargetCarryingCharacter(DialogLineData* self, Dialogue* dialog, Character* target)
	{
		if (target == nullptr)
			return false;

		for (auto iter = self->isTargetCarryingCharacter.begin(); iter != self->isTargetCarryingCharacter.end(); ++iter)
		{
			auto platoon = target->getPlatoon();
			for (auto squadMemberIter = platoon->things.begin(); squadMemberIter != platoon->things.end(); ++squadMemberIter)
			{
				auto squadMember = static_cast<Character*>(*squadMemberIter);
				auto carrying = squadMember->carryingObject.getCharacter();
				if (carrying != nullptr && carrying->getGameData() == *iter)
				{
					if (squadMember != target)
						if (!dialog->me->getSensoryData()->amIAwareOfThisGuy(squadMember, false))
							continue;

					dialog->changeConversationTarget(squadMember);
					return true;
				}
			}
		}
		return false;
	}

	bool _checkTargetHasItem(DialogLineData* self, Dialogue* dialog, Character* target)
	{
		if (target == nullptr)
			return false;

		bool matchItemFunction = self->hasItemType == ITEM_NO_FUNCTION;
		bool matchItem = self->hasItem.size() == 0;

		auto platoon = target->getPlatoon();
		for (auto squadMemberIter = platoon->things.begin(); squadMemberIter != platoon->things.end(); ++squadMemberIter)
		{
			auto squadMember = static_cast<Character*>(*squadMemberIter);
			if (squadMember != target)
				if (!dialog->me->getSensoryData()->amIAwareOfThisGuy(squadMember, false))
					continue;

			if (self->hasItemType != ITEM_NO_FUNCTION && !matchItemFunction)
			{
				if (squadMember->getInventory()->hasItemFunction(self->hasItemType))
				{
					matchItemFunction = true;
				}
				else
				{
					auto backpack = squadMember->hasABackpackOn();
					if (backpack != nullptr && backpack->getInventory()->hasItemFunction(self->hasItemType))
						matchItemFunction = true;
				}
			}

			for (auto iter = self->hasItem.begin(); iter != self->hasItem.end(); ++iter)
			{
				if (squadMember->getInventory()->hasItem(*iter, 1))
				{
					matchItem = true;
				}
				else
				{
					auto backpack = squadMember->hasABackpackOn();
					if (backpack != nullptr && backpack->getInventory()->hasItem(*iter, 1))
						matchItem = true;
				}
			}

			if (matchItem && matchItemFunction)
			{
				dialog->changeConversationTarget(squadMember);
				break;
			}
		}

		return matchItem && matchItemFunction;
	}

	bool _isForSpecificRace(DialogLineData* self, Dialogue* dialog)
	{
		auto race = dialog->me->getRace();
		for (auto iter = self->isMyRace.begin(); iter != self->isMyRace.end(); ++iter)
		{
			if (race->isRelatedRace(*iter))
				return true;
		}
		return false;
	}

	bool _isForSpecificSubRace(DialogLineData* self, Dialogue* dialog)
	{
		auto race = dialog->me->getRace();
		for (auto iter = self->isMyRace.begin(); iter != self->isMyRace.end(); ++iter)
		{
			if (race->data == *iter)
				return true;
		}
		return false;
	}

	bool _checkTargetRace(DialogLineData* self, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers)
	{
		if (target == nullptr)
			return false;

		auto race = target->getRace();
		for (auto iter = self->isTargetRace.begin(); iter != self->isTargetRace.end(); ++iter)
		{
			if (race->isRelatedRace(*iter))
				return true;
		}

		for (auto iter = self->isTargetRace.begin(); iter != self->isTargetRace.end(); ++iter)
		{
			auto platoon = target->getPlatoon();
			for (auto squadMemberIter = platoon->things.begin(); squadMemberIter != platoon->things.end(); ++squadMemberIter)
			{
				auto squadMember = static_cast<Character*>(*squadMemberIter);
				if (squadMember->getRace()->isRelatedRace(*iter))
				{
					if (squadMember != target)
						if (!dialog->me->getSensoryData()->amIAwareOfThisGuy(squadMember, false))
							continue;

					if (!isWordswap)
						dialog->changeConversationTarget(squadMember);
					return true;
				}
			}
		}
		return false;
	}

	bool _checkTargetSubRace(DialogLineData* self, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers)
	{
		if (target == nullptr)
			return false;

		auto race = target->getRace();
		for (auto iter = self->isTargetSubRace_specificallyTheTarget.begin(); iter != self->isTargetSubRace_specificallyTheTarget.end(); ++iter)
		{
			if (race->data == *iter)
				return true;
		}

		for (auto iter = self->isTargetSubRace_specificallyTheTarget.begin(); iter != self->isTargetSubRace_specificallyTheTarget.end(); ++iter)
		{
			auto platoon = target->getPlatoon();
			for (auto squadMemberIter = platoon->things.begin(); squadMemberIter != platoon->things.end(); ++squadMemberIter)
			{
				auto squadMember = static_cast<Character*>(*squadMemberIter);
				if (squadMember->getRace()->data == *iter)
				{
					if (squadMember != target)
						if (!dialog->me->getSensoryData()->amIAwareOfThisGuy(squadMember, false))
							continue;

					if (!isWordswap)
						dialog->changeConversationTarget(squadMember);
					return true;
				}
			}
		}
		return false;
	}

	bool DialogLineData_checkConditionsEx(DialogLineData* self, Dialogue* dialog, Character* target, bool isWordswap)
	{
		auto me = dialog->me;

		//auto checkTagTarget = target;
		//if (checkTagTarget != nullptr && dialog->me == checkTagTarget)
		//	checkTagTarget = dialog->conversationMaster.getCharacter();

		if (!self->checkTags(me, target))
			return false;

		if (self->oneAtATime)
		{
			auto platoon = me->getPlatoon();
			for (auto iter = platoon->things.begin(); iter != platoon->things.end(); ++iter)
			{
				auto dialogueMe = static_cast<Character*>(*iter)->dialogue;
				if (dialogueMe != nullptr && dialogueMe->currentConversation == self && !dialogueMe->conversationHasEnded())
					return false;
			}
		}

		if (dialog->isLocked(self))
			return false;

		if (self->unique && self->uniqueOwner.type != NULL_ITEM)
		{
			if (self->uniqueOwner != me->getHandle())
				return false;
		}

		if (self->chanceTemporary < 99.0f)
		{
			if (self->chanceTemporary < UtilityT::random(0.0f, 100.0f))
				return false;
		}

		bool ignoreTargetMembers = self->data->bdata["ignore target members"];

		auto lineDataExtend = lineDataExtends[self];
		if (lineDataExtend != nullptr && !lineDataExtend->isInArea(dialog))
			return false;

		if (self->inTownOf.size() != 0)
		{
			if (!dialog->isAtTownOf(self->inTownOf))
				return false;
		}

		if (lineDataExtend != nullptr && !lineDataExtend->isInSpecificTown(dialog))
			return false;

		if (lineDataExtend != nullptr && !lineDataExtend->isInReplacementTown(dialog))
			return false;

		if (self->isMyFaction.size() != 0)
		{
			if (self->isMyFaction.count(me->getFaction()) == 0)
				return false;
		}

		if (self->isTargetFaction.size() != 0)
		{
			if (target == nullptr)
				return false;

			if (self->isTargetFaction.count(target->getFaction()) == 0)
				return false;
		}

		if (self->_hasPackage.size() != 0)
		{
			if (!_isForSpecificPackage(self, dialog))
				return false;
		}

		if (self->isCharacter.size() != 0)
		{
			if (!self->isForSpecificCharacter(me->getGameData()))
				return false;
		}

		if (lineDataExtend != nullptr && !lineDataExtend->checkTargetCharacter(dialog, target, isWordswap, ignoreTargetMembers))
			return false;

		if (lineDataExtend != nullptr && !lineDataExtend->checkActualTargetCharacter(target))
			return false;

		if (self->isTargetCarryingCharacter.size() != 0)
		{
			if (!_checkTargetCarryingCharacter(self, dialog, target))
				return false;
		}

		if (self->worldState != nullptr)
		{
			if (!self->worldState->isTrue())
				return false;
		}

		if (self->hasItem.size() != 0 || self->hasItemType != ITEM_NO_FUNCTION)
		{
			if (!_checkTargetHasItem(self, dialog, target))
				return false;
		}

		if (self->isTargetRace.size() != 0)
		{
			if (!_checkTargetRace(self, dialog, target, isWordswap, ignoreTargetMembers))
				return false;
		}

		if (self->isTargetSubRace_specificallyTheTarget.size() != 0)
		{
			if (!_checkTargetSubRace(self, dialog, target, isWordswap, ignoreTargetMembers))
				return false;
		}

		if (lineDataExtend != nullptr && !lineDataExtend->checkActualTargetRace(target))
			return false;

		if (lineDataExtend != nullptr && !lineDataExtend->checkActualTargetSubRace(target))
			return false;

		if (self->isMyRace.size() != 0)
		{
			if (!_isForSpecificRace(self, dialog))
				return false;
		}

		if (self->isMySubRace.size() != 0)
		{
			if (!_isForSpecificSubRace(self, dialog))
				return false;
		}

		if (!self->checkRepeatLimits())
			return false;

		if (0 < self->forCertainType)
		{
			if (target == nullptr)
				return false;

			if (self->forCertainType == OT_SLAVE)
			{
				auto s = target->isSlave();
				return (s == IS_SLAVE || s == ESCAPING_SLAVE || target->stateBroadcast->NPCType == self->forCertainType);
			}
			if (target->stateBroadcast->NPCType != self->forCertainType)
				return false;
		}

		if (self->chancePermanent < 99.0f)
		{
			if (!dialog->hasThisChanceLine(self, self->chancePermanent))
				return false;

			if (self->unique)
				self->uniqueOwner = me->getHandle();
		}

		for (auto iter = self->conditions.begin(); iter != self->conditions.end(); ++iter)
		{
			auto con = *iter;
			Character* conTarget = target;

			if (me == target && !isWordswap)
			{
				if (con->who == T_ME)
				{
					conTarget = dialog->conversationMaster.getCharacter();
					if (conTarget == nullptr)
						conTarget = me;
				}
			}
			else
			{
				if (con->who == T_ME)
					conTarget = me;
			}

			if (!dialog->_checkCondition(con->key, con->compareBy, con->value, conTarget, target))
				return false;
		}

		return true;
	}

	bool (*DialogLineData_checkConditions_orig)(DialogLineData*, Dialogue*, Character*, bool);
	bool DialogLineData_checkConditions_hook(DialogLineData* self, Dialogue* dialog, Character* target, bool isWordswap)
	{
		if (KEP::settings._dialogueConditionEx)
			return DialogLineData_checkConditionsEx(self, dialog, target, isWordswap);

		return DialogLineData_checkConditions_orig(self, dialog, target, isWordswap);
	}

	bool (*Dialogue__checkCondition_orig)(Dialogue*, DialogConditionEnum, ComparisonEnum, int, Character*, Character*);
	bool Dialogue__checkCondition_hook(Dialogue* self, DialogConditionEnum conditionName, ComparisonEnum compareBy, int val, Character* target, Character* actualConversationTarget)
	{
		if (KEP::settings._dialogueConditionEx)
		{
			switch (conditionName)
			{
			case DC_IS_SAME_SUBRACE_AS_ME:
			case DC_IS_ANIMAL_RACE:
			case DC_IS_ROBOT_RACE:
			{
				if (target == nullptr)
				{
					Character* character = ou->player->getNearestSelectedCharacterTo(self->me->getPosition());
					if (character == nullptr)
					{
						auto playerActivePlatoon = ou->player->getCurrentActivePlatoon();
						if (playerActivePlatoon->things.size() != 0)
							character = static_cast<Character*>(playerActivePlatoon->things[0]);
					}
					if (character != nullptr)
						self->conversationTarget = character->handle;
					target = character;
				}
				if (target == nullptr)
					return true;
				break;
			}
			default:
				return Dialogue__checkCondition_orig(self, conditionName, compareBy, val, target, actualConversationTarget);
			}

			bool booleanCondition = 0 < val;

			if (conditionName == DC_IS_SAME_SUBRACE_AS_ME)
			{
				Character* temp = target;
				self->dontLetTargetBeMe(&temp, actualConversationTarget);
				target = temp;

				return (self->me->getRace() == target->getRace()) == booleanCondition;
			}
			else if (conditionName == DC_IS_ANIMAL_RACE)
			{
				return target->isAnimal() != nullptr == booleanCondition;
			}
			else if (conditionName == DC_IS_ROBOT_RACE)
			{
				return target->getRace()->robot == booleanCondition;
			}
		}
		return Dialogue__checkCondition_orig(self, conditionName, compareBy, val, target, actualConversationTarget);
	}

	Character* (*Dialogue_getSpeaker_orig)(Dialogue*, TalkerEnum, DialogLineData*, bool);
	Character* Dialogue_getSpeaker_hook(Dialogue* self, TalkerEnum who, DialogLineData* line, bool isForWordswaps)
	{
		if (KEP::settings._dialogueConditionEx && who == T_TARGET_WITH_RACE)
		{
			if (line == nullptr)
				return nullptr;

			auto target = self->conversationTarget.getCharacter();
			if (target != nullptr)
			{
				auto race = target->getRace();
				for (auto iter = line->isTargetSubRace_specificallyTheTarget.begin(); iter != line->isTargetSubRace_specificallyTheTarget.end(); ++iter)
				{
					if (race->data == *iter)
						return target;
				}

				for (auto iter = line->isTargetRace.begin(); iter != line->isTargetRace.end(); ++iter)
				{
					if (race->isRelatedRace(*iter))
						return target;
				}
			}
			return nullptr;
		}
		return Dialogue_getSpeaker_orig(self, who, line, isForWordswaps);
	}

	int (*DialogLineData_getScore_orig)(DialogLineData*, Character*);
	int DialogLineData_getScore_hook(DialogLineData* self, Character* target)
	{
		if (!KEP::settings._dialogueConditionEx)
			return DialogLineData_getScore_orig(self, target);

		if (target != nullptr)
		{
			auto race = target->getRace();
			for (auto iter = self->isTargetSubRace_specificallyTheTarget.begin(); iter != self->isTargetSubRace_specificallyTheTarget.end(); ++iter)
			{
				if (race->data == *iter)
					return self->score + 2;
			}
			for (auto iter = self->isTargetRace.begin(); iter != self->isTargetRace.end(); ++iter)
			{
				if (race->isRelatedRace(*iter))
					return self->score + 2;
			}
		}

		return self->score;
	}
}

void KEP::DialogueConditionEx::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&DialogLineData::_CONSTRUCTOR), &DialogLineData__CONSTRUCTOR_hook, &DialogLineData__CONSTRUCTOR_orig))
		ErrorLog("[DialogLineData::DialogLineData] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&DialogLineData::_DESTRUCTOR), &DialogLineData__DESTRUCTOR_hook, &DialogLineData__DESTRUCTOR_orig))
		ErrorLog("[DialogLineData::~DialogLineData] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&Dialogue::_checkCondition), &Dialogue__checkCondition_hook, &Dialogue__checkCondition_orig))
		ErrorLog("[Dialogue::_checkCondition] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&DialogLineData::checkConditions), &DialogLineData_checkConditions_hook, &DialogLineData_checkConditions_orig))
		ErrorLog("[DialogLineData::checkConditions] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&Dialogue::getSpeaker), &Dialogue_getSpeaker_hook, &Dialogue_getSpeaker_orig))
		ErrorLog("[Dialogue::getSpeaker] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&DialogLineData::getScore), &DialogLineData_getScore_hook, &DialogLineData_getScore_orig))
		ErrorLog("[DialogLineData::getScore] could not install hook!");
}
