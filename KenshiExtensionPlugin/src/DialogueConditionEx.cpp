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

	private:
		void _initialiseAList(const std::string& listname, lektor<GameData*>& list) const;
		DialogLineData* line;
		lektor<GameData*> isTargetCharacter;
		lektor<GameData*> inAreaOf;
		lektor<GameData*> inSpecificTownOf;
		lektor<GameData*> inReplacementTownOf;
		lektor<GameData*> targetHasPackage;

	public:
		DialogLineData* getLine() const { return line; }
		const lektor<GameData*>& getIsTargetCharacter() const { return isTargetCharacter; }
		const lektor<GameData*>& getInAreaOf() const { return inAreaOf; }
		const lektor<GameData*>& getInSpecificTownOf() const { return inSpecificTownOf; }
		const lektor<GameData*>& getInReplacementTownOf() const { return inReplacementTownOf; }
		const lektor<GameData*>& getTargetHasPackage() const { return targetHasPackage; }
	};

	DialogLineDataExtend::DialogLineDataExtend(DialogLineData* _line)
		: line(_line)
	{
		_initialiseAList("target character", isTargetCharacter);
		_initialiseAList("in area of", inAreaOf);
		_initialiseAList("in specific town of", inSpecificTownOf);
		_initialiseAList("in replacement town of", inReplacementTownOf);
		_initialiseAList("target has package", targetHasPackage);

		if (isTargetCharacter.size() != 0)
			line->score += 2;
		if (inAreaOf.size() != 0)
			line->score += 2;
		if (inSpecificTownOf.size() != 0)
			line->score += 2;
		if (inReplacementTownOf.size() != 0)
			line->score += 2;
		if (targetHasPackage.size() != 0)
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
			float repeatTime = self->data->fdata["repeat time"];
			if (0.0f < repeatTime && repeatTime < 5000.0f)
				self->dialogRepeatMinTimeInHours = repeatTime;

			self->_initialiseAList("target subrace", &self->isTargetSubRace_specificallyTheTarget);

			lineDataExtends[self] = new DialogLineDataExtend(self);
		}
		return self;
	}

	bool (*DialogLineData_checkConditions_orig)(DialogLineData*, Dialogue*, Character*, bool);
	bool DialogLineData_checkConditions_hook(DialogLineData* self, Dialogue* dialog, Character* target, bool isWordswap)
	{
		if (!KEP::settings._dialogueConditionEx)
			return DialogLineData_checkConditions_orig(self, dialog, target, isWordswap);

		auto me = dialog->me;

		//auto checkTagTarget = target;
		//if (!isWordswap && dialog->conversationMaster != me->getHandle())
		//{
		//	auto conversationMaster = dialog->conversationMaster.getCharacter();
		//	if (conversationMaster != nullptr)
		//		checkTagTarget = conversationMaster;
		//}

		if (!self->checkTags(me, target))
			return false;

		if (!KEP::DialogueConditionEx::checkOneAtATime(self, dialog, target, isWordswap))
			return false;

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
		if (lineDataExtend != nullptr && !KEP::DialogueConditionEx::checkArea(self, dialog, target, isWordswap, lineDataExtend->getInAreaOf()))
			return false;

		if (!KEP::DialogueConditionEx::checkTown(self, dialog, target, isWordswap))
			return false;

		if (lineDataExtend != nullptr && !KEP::DialogueConditionEx::checkSpecificTown(self, dialog, target, isWordswap, lineDataExtend->getInSpecificTownOf()))
			return false;

		if (lineDataExtend != nullptr && !KEP::DialogueConditionEx::checkReplacementTown(self, dialog, target, isWordswap, lineDataExtend->getInReplacementTownOf()))
			return false;

		if (!KEP::DialogueConditionEx::checkMyFaction(self, dialog, target, isWordswap))
			return false;

		if (!KEP::DialogueConditionEx::checkTargetFaction(self, dialog, target, isWordswap))
			return false;

		if (!KEP::DialogueConditionEx::checkDialoguePackage(self, dialog, target, isWordswap))
			return false;

		if (!KEP::DialogueConditionEx::checkSpecificCharacter(self, dialog, target, isWordswap))
			return false;

		if (lineDataExtend != nullptr && !KEP::DialogueConditionEx::checkTargetDialoguePackage(self, dialog, target, isWordswap, ignoreTargetMembers, lineDataExtend->getTargetHasPackage()))
			return false;

		if (lineDataExtend != nullptr && !KEP::DialogueConditionEx::checkTargetSpecificCharacter(self, dialog, target, isWordswap, ignoreTargetMembers, lineDataExtend->getIsTargetCharacter()))
			return false;

		if (!KEP::DialogueConditionEx::checkTargetCarryingCharacter(self, dialog, target, isWordswap, ignoreTargetMembers))
			return false;

		if (!KEP::DialogueConditionEx::checkWorldState(self, dialog, target, isWordswap))
			return false;

		if (!KEP::DialogueConditionEx::checkTargetHasItem(self, dialog, target, isWordswap, ignoreTargetMembers))
			return false;

		if (!KEP::DialogueConditionEx::checkTargetRace(self, dialog, target, isWordswap, ignoreTargetMembers))
			return false;

		if (!KEP::DialogueConditionEx::checkTargetSubRace(self, dialog, target, isWordswap, ignoreTargetMembers))
			return false;

		if (!KEP::DialogueConditionEx::checkMyRace(self, dialog, target, isWordswap))
			return false;

		if (!KEP::DialogueConditionEx::checkMySubRace(self, dialog, target, isWordswap))
			return false;

		if (!self->checkRepeatLimits())
			return false;

		if (!KEP::DialogueConditionEx::checkTargetType(self, dialog, target, isWordswap))
			return false;

		if (self->chancePermanent < 99.0f)
		{
			if (!dialog->hasThisChanceLine(self, self->chancePermanent))
				return false;

			if (self->unique)
				self->uniqueOwner = me->getHandle();
		}

		auto endConditions = self->conditions.end();
		for (auto iter = self->conditions.begin(); iter != endConditions; ++iter)
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

			if (!KEP::DialogueConditionEx::checkDialogCondition(self, dialog, con->key, con->compareBy, con->who, con->value, conTarget, target))
				return false;
		}

		return true;
	}

	Character* (*Dialogue_getSpeaker_orig)(Dialogue*, TalkerEnum, DialogLineData*, bool);
	Character* Dialogue_getSpeaker_hook(Dialogue* self, TalkerEnum who, DialogLineData* line, bool isForWordswaps)
	{
		if (KEP::settings._dialogueConditionEx)
		{
			if (who == T_TARGET_IF_PLAYER)
			{
				if (isForWordswaps && self->getHandle() == self->conversationTarget)
				{
					auto target = self->conversationMaster.getCharacter();
					if (target != nullptr && self->currentConversationType != EV_NONE && target->isPlayerCharacter())
						return target;
				}

				auto target = self->conversationTarget.getCharacter();
				if (target != nullptr && target->isPlayerCharacter())
					return target;

				return nullptr;
			}
			else if (who == T_TARGET_WITH_RACE)
			{
				if (line == nullptr)
					return nullptr;

				auto platoon = self->conversationTarget.getPlatoon();
				auto activePlatoon = platoon != nullptr ? platoon->activePlatoon : nullptr;
				if (activePlatoon == nullptr)
					return nullptr;

				for (auto iter = line->isTargetSubRace_specificallyTheTarget.begin(); iter != line->isTargetSubRace_specificallyTheTarget.end(); ++iter)
				{
					for (auto squadMemberIter = activePlatoon->things.begin(); squadMemberIter != activePlatoon->things.end(); ++squadMemberIter)
					{
						auto squadMember = static_cast<Character*>(*squadMemberIter);
						if (squadMember->getRace()->isRelatedRace(*iter))
							return squadMember;
					}
				}

				for (auto iter = line->isTargetRace.begin(); iter != line->isTargetRace.end(); ++iter)
				{
					for (auto squadMemberIter = activePlatoon->things.begin(); squadMemberIter != activePlatoon->things.end(); ++squadMemberIter)
					{
						auto squadMember = static_cast<Character*>(*squadMemberIter);
						if (squadMember->getRace()->data == *iter)
							return squadMember;
					}
				}

				return nullptr;
			}
			else if (who == T_TARGET_IF_ANIMAL)
			{
				if (isForWordswaps && self->getHandle() == self->conversationTarget)
				{
					auto target = self->conversationMaster.getCharacter();
					if (target != nullptr && self->currentConversationType != EV_NONE && target->isAnimal() != nullptr)
						return target;
				}

				auto target = self->conversationTarget.getCharacter();
				if (target != nullptr && target->isAnimal() != nullptr)
					return target;

				return nullptr;
			}
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

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&DialogLineData::checkConditions), &DialogLineData_checkConditions_hook, &DialogLineData_checkConditions_orig))
		ErrorLog("[DialogLineData::checkConditions] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&Dialogue::getSpeaker), &Dialogue_getSpeaker_hook, &Dialogue_getSpeaker_orig))
		ErrorLog("[Dialogue::getSpeaker] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&DialogLineData::getScore), &DialogLineData_getScore_hook, &DialogLineData_getScore_orig))
		ErrorLog("[DialogLineData::getScore] could not install hook!");
}

bool KEP::DialogueConditionEx::checkOneAtATime(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap)
{
	if (line->oneAtATime)
	{
		auto platoon = dialog->me->getPlatoon();
		for (auto iter = platoon->things.begin(); iter != platoon->things.end(); ++iter)
		{
			auto dialogueMe = static_cast<Character*>(*iter)->dialogue;
			if (dialogueMe != nullptr && dialogueMe->currentConversation == line && !dialogueMe->conversationHasEnded())
				return false;
		}
	}
	return true;
}

bool KEP::DialogueConditionEx::checkTown(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap)
{
	return line->inTownOf.size() == 0 || dialog->isAtTownOf(line->inTownOf);
}

bool KEP::DialogueConditionEx::checkMyFaction(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap)
{
	return line->isMyFaction.size() == 0 || line->isMyFaction.count(dialog->me->getFaction()) != 0;
}

bool KEP::DialogueConditionEx::checkTargetFaction(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap)
{
	return line->isTargetFaction.size() == 0 || target != nullptr && line->isTargetFaction.count(target->getFaction()) != 0;
}

bool KEP::DialogueConditionEx::checkDialoguePackage(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap)
{
	if (line->_hasPackage.size() == 0)
		return true;

	const auto& pacakgesIHave = dialog->pacakgesIHave;
	auto endPackages = line->_hasPackage.end();
	for (auto iter = line->_hasPackage.begin(); iter != endPackages; ++iter)
	{
		if (pacakgesIHave.count(*iter) != 0)
			return true;
	}
	return false;
}

bool KEP::DialogueConditionEx::checkSpecificCharacter(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap)
{
	return line->isCharacter.size() == 0 || line->isForSpecificCharacter(dialog->me->getGameData());
}

bool KEP::DialogueConditionEx::checkTargetCarryingCharacter(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers)
{
	const auto& isTargetCarryingCharacter = line->isTargetCarryingCharacter;
	if (isTargetCarryingCharacter.size() == 0)
		return true;

	if (target == nullptr)
		return false;

	auto endIsTargetCarryingCharacter = isTargetCarryingCharacter.end();
	if (ignoreTargetMembers)
	{
		auto carrying = target->carryingObject.getCharacter();
		for (auto charaterIt = isTargetCarryingCharacter.begin(); charaterIt != endIsTargetCarryingCharacter; ++charaterIt)
		{
			if (carrying != nullptr && carrying->getGameData() == *charaterIt)
				return true;
		}
		return false;
	}

	auto speaker = dialog->me;
	const auto& squadMembers = target->getPlatoon()->things;
	auto endSquadMembers = squadMembers.end();

	for (auto charaterIt = isTargetCarryingCharacter.begin(); charaterIt != endIsTargetCarryingCharacter; ++charaterIt)
	{
		for (auto squadMemberIt = squadMembers.begin(); squadMemberIt != endSquadMembers; ++squadMemberIt)
		{
			auto squadMember = static_cast<Character*>(*squadMemberIt);
			auto carrying = squadMember->carryingObject.getCharacter();
			if (carrying != nullptr && carrying->getGameData() == *charaterIt)
			{
				if (squadMember != target)
					if (!speaker->getSensoryData()->amIAwareOfThisGuy(squadMember, false))
						continue;

				dialog->changeConversationTarget(squadMember);
				return true;
			}
		}
	}
	return false;
}

bool KEP::DialogueConditionEx::checkWorldState(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap)
{
	return line->worldState == nullptr || line->worldState->isTrue();
}

bool KEP::DialogueConditionEx::checkTargetHasItem(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers)
{
	const auto& hasItem = line->hasItem;
	auto hasItemType = line->hasItemType;

	bool matchItemFunction = hasItemType == ITEM_NO_FUNCTION;
	bool matchItem = hasItem.size() == 0;

	if (matchItem && matchItemFunction)
		return true;

	if (target == nullptr)
		return false;

	auto endHasItem = hasItem.end();
	if (ignoreTargetMembers)
	{
		auto inventory = target->getInventory();
		auto backpack = target->hasABackpackOn();
		auto backpackInventory = backpack != nullptr ? backpack->getInventory() : nullptr;

		if (hasItemType != ITEM_NO_FUNCTION && !matchItemFunction)
		{
			if (inventory->hasItemFunction(hasItemType))
				matchItemFunction = true;
			else
				if (backpackInventory != nullptr && backpackInventory->hasItemFunction(hasItemType))
					matchItemFunction = true;
		}

		for (auto itemIt = hasItem.begin(); itemIt != endHasItem; ++itemIt)
		{
			if (inventory->hasItem(*itemIt, 1))
				matchItem = true;
			else
				if (backpackInventory != nullptr && backpackInventory->hasItem(*itemIt, 1))
					matchItem = true;
		}

		return matchItem && matchItemFunction;
	}

	auto speaker = dialog->me;
	const auto& squadMembers = target->getPlatoon()->things;
	auto endSquadMembers = squadMembers.end();

	for (auto squadMemberIt = squadMembers.begin(); squadMemberIt != endSquadMembers; ++squadMemberIt)
	{
		auto squadMember = static_cast<Character*>(*squadMemberIt);
		if (squadMember != target)
			if (!speaker->getSensoryData()->amIAwareOfThisGuy(squadMember, false))
				continue;

		auto inventory = squadMember->getInventory();
		auto backpack = squadMember->hasABackpackOn();
		auto backpackInventory = backpack != nullptr ? backpack->getInventory() : nullptr;

		if (hasItemType != ITEM_NO_FUNCTION && !matchItemFunction)
		{
			if (inventory->hasItemFunction(hasItemType))
				matchItemFunction = true;
			else
				if (backpackInventory != nullptr && backpackInventory->hasItemFunction(hasItemType))
					matchItemFunction = true;
		}

		for (auto itemIt = hasItem.begin(); itemIt != endHasItem; ++itemIt)
		{
			if (inventory->hasItem(*itemIt, 1))
				matchItem = true;
			else
				if (backpackInventory != nullptr && backpackInventory->hasItem(*itemIt, 1))
					matchItem = true;
		}

		if (matchItem && matchItemFunction)
		{
			if (!isWordswap)
				dialog->changeConversationTarget(squadMember);
			break;
		}
	}

	return matchItem && matchItemFunction;
}

bool KEP::DialogueConditionEx::checkTargetRace(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers)
{
	const auto& isTargetRace = line->isTargetRace;
	if (isTargetRace.size() == 0)
		return true;

	if (target == nullptr)
		return false;

	auto race = target->getRace();
	auto endIsTargetRace = isTargetRace.end();
	for (auto raceIt = isTargetRace.begin(); raceIt != endIsTargetRace; ++raceIt)
	{
		if (race->isRelatedRace(*raceIt))
			return true;
	}

	if (ignoreTargetMembers)
		return false;

	auto speaker = dialog->me;
	const auto& squadMembers = target->getPlatoon()->things;
	auto endSquadMembers = squadMembers.end();

	for (auto raceIt = isTargetRace.begin(); raceIt != endIsTargetRace; ++raceIt)
	{
		for (auto squadMemberIt = squadMembers.begin(); squadMemberIt != endSquadMembers; ++squadMemberIt)
		{
			auto squadMember = static_cast<Character*>(*squadMemberIt);
			if (squadMember->getRace()->isRelatedRace(*raceIt))
			{
				if (squadMember != target)
					if (!speaker->getSensoryData()->amIAwareOfThisGuy(squadMember, false))
						continue;

				if (!isWordswap)
					dialog->changeConversationTarget(squadMember);
				return true;
			}
		}
	}
	return false;
}

bool KEP::DialogueConditionEx::checkTargetSubRace(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers)
{
	const auto& isTargetSubRace_specificallyTheTarget = line->isTargetSubRace_specificallyTheTarget;
	if (isTargetSubRace_specificallyTheTarget.size() == 0)
		return true;

	if (target == nullptr)
		return false;

	auto gd = target->getRace()->data;
	auto endIsTargetSubRace_specificallyTheTargete = isTargetSubRace_specificallyTheTarget.end();
	for (auto raceIt = isTargetSubRace_specificallyTheTarget.begin(); raceIt != endIsTargetSubRace_specificallyTheTargete; ++raceIt)
	{
		if (gd == *raceIt)
			return true;
	}

	if (ignoreTargetMembers)
		return false;

	auto speaker = dialog->me;
	const auto& squadMembers = target->getPlatoon()->things;
	auto endSquadMembers = squadMembers.end();

	for (auto iter = isTargetSubRace_specificallyTheTarget.begin(); iter != endIsTargetSubRace_specificallyTheTargete; ++iter)
	{
		auto platoon = target->getPlatoon();
		for (auto squadMemberIt = squadMembers.begin(); squadMemberIt != endSquadMembers; ++squadMemberIt)
		{
			auto squadMember = static_cast<Character*>(*squadMemberIt);
			if (squadMember->getRace()->data == *iter)
			{
				if (squadMember != target)
					if (!speaker->getSensoryData()->amIAwareOfThisGuy(squadMember, false))
						continue;

				if (!isWordswap)
					dialog->changeConversationTarget(squadMember);
				return true;
			}
		}
	}
	return false;
}

bool KEP::DialogueConditionEx::checkMyRace(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap)
{
	const auto& isMyRace = line->isMyRace;
	if (isMyRace.size() == 0)
		return true;

	auto race = dialog->me->getRace();
	auto endMyRace = isMyRace.end();
	for (auto iter = isMyRace.begin(); iter != endMyRace; ++iter)
	{
		if (race->isRelatedRace(*iter))
			return true;
	}
	return false;
}

bool KEP::DialogueConditionEx::checkMySubRace(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap)
{
	const auto& isMySubRace = line->isMySubRace;
	if (isMySubRace.size() == 0)
		return true;

	auto race = dialog->me->getRace();
	auto endMySubRace = isMySubRace.end();
	for (auto iter = isMySubRace.begin(); iter != endMySubRace; ++iter)
	{
		if (race->data == *iter)
			return true;
	}
	return false;
}

bool KEP::DialogueConditionEx::checkTargetType(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap)
{
	if (line->forCertainType == OT_NONE)
		return true;

	if (target == nullptr)
		return false;

	if (line->forCertainType == OT_SLAVE)
	{
		auto slaveState = target->isSlave();
		return (slaveState == IS_SLAVE || slaveState == ESCAPING_SLAVE || target->stateBroadcast->NPCType == line->forCertainType);
	}

	return target->stateBroadcast->NPCType == line->forCertainType;
}

bool KEP::DialogueConditionEx::checkDialogCondition(DialogLineData* line, Dialogue* dialog, DialogConditionEnum conditionName, ComparisonEnum compareBy, TalkerEnum who, int val, Character* target, Character* actualConversationTarget)
{
	auto speaker = dialog->me;
	switch (conditionName)
	{
	case DC_HAS_ILLEGAL_ITEM:
	case DC_IS_SAME_SUBRACE_AS_ME:
	case DC_IS_ANIMAL_RACE:
	case DC_IS_ROBOT_RACE:
	case DC_SQUAD_MONEY:
	{
		if (target == nullptr)
		{
			Character* character = ou->player->getNearestSelectedCharacterTo(speaker->getPosition());
			if (character == nullptr)
			{
				auto playerActivePlatoon = ou->player->getCurrentActivePlatoon();
				if (playerActivePlatoon->things.size() != 0)
					character = static_cast<Character*>(playerActivePlatoon->things[0]);
			}
			if (character != nullptr)
				dialog->conversationTarget = character->handle;
			target = character;
		}
		if (target == nullptr)
			return true;
		break;
	}
	default:
		return dialog->_checkCondition(conditionName, compareBy, val, target, actualConversationTarget);
	}

	switch (conditionName)
	{
	case DC_IS_SAME_SUBRACE_AS_ME:
	{
		Character* temp = target;
		dialog->dontLetTargetBeMe(&temp, actualConversationTarget);
		target = temp;
		break;
	}
	}

	bool booleanCondition = 0 < val;

	switch (conditionName)
	{
	case DC_HAS_ILLEGAL_ITEM:
	{
		const auto msg_smuggling = "{1} has been discovered smuggling {2}";
		auto town = speaker->getCurrentTownLocation();
		auto faction = town != nullptr ? town->getFaction() : speaker->getFaction();

		auto platoon = target->getPlatoon();
		if (platoon == nullptr)
			return false;

		int match = 0;

		const auto& illegalItems = faction->tradeCulture.illegalItems;
		auto endIllegalItems = illegalItems.end();
		if (line->data->bdata["ignore target members"])
		{
			auto inventory = target->getInventory();
			auto backpack = target->hasABackpackOn();
			auto backpackInventory = backpack != nullptr ? backpack->getInventory() : nullptr;
			for (auto itemIt = illegalItems.begin(); itemIt != endIllegalItems; ++itemIt)
			{
				auto item = *itemIt;
				if (!inventory->hasItem(item, 1))
				{
					if (!KEP::settings._fixSmugglingCheck || backpackInventory == nullptr || !backpackInventory->hasItem(item, 1))
						continue;
				}

				ou->showPlayerAMessage_withLog(
					KEP::TranslationUtility::format_main(boost::locale::format(boost::locale::translate(msg_smuggling)) % target->getName() % (item)->name),
					false
				);
				match = 1;
				break;
			}
		}
		else
		{
			const auto& squadMembers = platoon->things;
			auto endSquadMembers = squadMembers.end();
			for (auto squadMemberIt = squadMembers.begin(); squadMemberIt != endSquadMembers; ++squadMemberIt)
			{
				auto squadMember = static_cast<Character*>(*squadMemberIt);
				if (squadMember != target)
					if (!speaker->getSensoryData()->amIAwareOfThisGuy(squadMember, false))
						continue;

				auto inventory = squadMember->getInventory();
				auto backpack = squadMember->hasABackpackOn();
				auto backpackInventory = backpack != nullptr ? backpack->getInventory() : nullptr;
				for (auto itemIt = illegalItems.begin(); itemIt != endIllegalItems; ++itemIt)
				{
					auto item = *itemIt;
					if (!inventory->hasItem(item, 1))
					{
						if (!KEP::settings._fixSmugglingCheck || backpackInventory == nullptr || !backpackInventory->hasItem(item, 1))
							continue;
					}

					if (squadMember->isAnimal() == nullptr)
						dialog->changeConversationTarget(squadMember);

					ou->showPlayerAMessage_withLog(
						KEP::TranslationUtility::format_main(boost::locale::format(boost::locale::translate(msg_smuggling)) % squadMember->getName() % (item)->name),
						false
					);
					match = 1;
					break;
				}
			}
		}

		if (compareBy == CE_LESS_THAN)
			return match < val;
		else
			return val <= match;
	}
	case DC_IS_SAME_SUBRACE_AS_ME:
		return (speaker->getRace() == target->getRace()) == booleanCondition;
	case DC_IS_ANIMAL_RACE:
		return target->isAnimal() != nullptr == booleanCondition;
	case DC_IS_ROBOT_RACE:
		return target->getRace()->robot == booleanCondition;
	case DC_SQUAD_MONEY:
	{
		int money = target->getOwnerships()->money;
		return compareBy == CE_LESS_THAN ? money < val : val <= money;
	}
	}
	return dialog->_checkCondition(conditionName, compareBy, val, target, actualConversationTarget);
}

bool KEP::DialogueConditionEx::checkTargetDialoguePackage(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers, const lektor<GameData*>& packages)
{
	if (packages.size() == 0)
		return true;

	if (target == nullptr)
		return false;

	const auto& pacakgesIHave = target->dialogue->pacakgesIHave;
	auto endPackages = packages.end();
	for (auto packageIt = packages.begin(); packageIt != endPackages; ++packageIt)
	{
		if (pacakgesIHave.count(*packageIt) != 0)
			return true;
	}

	if (ignoreTargetMembers)
		return false;

	auto speaker = dialog->me;
	const auto& squadMembers = target->getPlatoon()->things;
	auto endSquadMembers = squadMembers.end();
	for (auto packageIt = packages.begin(); packageIt != endPackages; ++packageIt)
	{
		auto platoon = target->getPlatoon();
		for (auto squadMemberIt = squadMembers.begin(); squadMemberIt != endSquadMembers; ++squadMemberIt)
		{
			auto squadMember = static_cast<Character*>(*squadMemberIt);
			if (squadMember->dialogue->pacakgesIHave.count(*packageIt) != 0)
			{
				if (squadMember != target)
					if (!speaker->getSensoryData()->amIAwareOfThisGuy(squadMember, false))
						continue;

				if (!isWordswap)
					dialog->changeConversationTarget(squadMember);
				return true;
			}
		}
	}
	return false;
}

bool KEP::DialogueConditionEx::checkTargetSpecificCharacter(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers, const lektor<GameData*>& characters)
{
	if (characters.size() == 0)
		return true;

	if (target == nullptr)
		return false;

	auto gd = target->getGameData();
	auto endCharacters = characters.end();
	for (auto chracterIt = characters.begin(); chracterIt != endCharacters; ++chracterIt)
	{
		if (gd == *chracterIt)
			return true;
	}

	if (ignoreTargetMembers)
		return false;

	auto speaker = dialog->me;
	const auto& squadMembers = target->getPlatoon()->things;
	auto endSquadMembers = squadMembers.end();

	for (auto chracterIt = characters.begin(); chracterIt != endCharacters; ++chracterIt)
	{
		for (auto squadMemberIt = squadMembers.begin(); squadMemberIt != endSquadMembers; ++squadMemberIt)
		{
			auto squadMember = static_cast<Character*>(*squadMemberIt);
			if (squadMember->getGameData() == *chracterIt)
			{
				if (squadMember != target)
					if (!speaker->getSensoryData()->amIAwareOfThisGuy(squadMember, false))
						continue;

				if (!isWordswap)
					dialog->changeConversationTarget(squadMember);
				return true;
			}
		}
	}
	return false;
}

bool KEP::DialogueConditionEx::checkArea(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, const lektor<GameData*>& areas)
{
	if (areas.size() == 0)
		return true;

	auto biome = KEP::functions->AreasList_getBiome(shou->areasList, dialog->me->getPosition());
	if (biome == nullptr)
		return false;

	GameData* gd = biome->data;
	auto endAreas = areas.end();
	for (auto iter = areas.begin(); iter != endAreas; ++iter)
	{
		if (gd == *iter)
			return true;
	}
	return false;
}

bool KEP::DialogueConditionEx::checkSpecificTown(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, const lektor<GameData*>& towns)
{
	if (towns.size() == 0)
		return true;

	auto town = dialog->me->getCurrentTownLocation();
	if (town == nullptr)
		return false;

	GameData* gd = town->data;
	auto endTowns = towns.end();
	for (auto iter = towns.begin(); iter != endTowns; ++iter)
	{
		if (gd == *iter)
			return true;
	}
	return false;
}

bool KEP::DialogueConditionEx::checkReplacementTown(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, const lektor<GameData*>& towns)
{
	if (towns.size() == 0)
		return true;

	auto townBase = dialog->me->getCurrentTownLocation();
	if (townBase == nullptr)
		return false;

	auto town = townBase->isTown();
	if (town == nullptr)
		return false;

	GameData* gd = town->replacementTown;
	if (gd == nullptr)
		return false;

	auto endTowns = towns.end();
	for (auto iter = towns.begin(); iter != endTowns; ++iter)
	{
		if (gd == *iter)
			return true;
	}
	return false;
}
