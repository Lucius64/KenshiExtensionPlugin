/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#pragma once

template<typename T> class lektor;
class GameData;
class Character;
class Dialogue;
class DialogLineData;
enum DialogConditionEnum;
enum ComparisonEnum;
enum TalkerEnum;

namespace KEP
{
	namespace DialogueConditionEx
	{
		void init();
		__declspec(noinline) bool checkOneAtATime(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap);
		__declspec(noinline) bool checkTown(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap);
		__declspec(noinline) bool checkMyFaction(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap);
		__declspec(noinline) bool checkTargetFaction(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap);
		__declspec(noinline) bool checkDialoguePackage(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap);
		__declspec(noinline) bool checkSpecificCharacter(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap);
		__declspec(noinline) bool checkTargetCarryingCharacter(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers);
		__declspec(noinline) bool checkWorldState(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap);
		__declspec(noinline) bool checkTargetHasItem(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers);
		__declspec(noinline) bool checkTargetRace(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers);
		__declspec(noinline) bool checkTargetSubRace(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers);
		__declspec(noinline) bool checkMyRace(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap);
		__declspec(noinline) bool checkMySubRace(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap);
		__declspec(noinline) bool checkTargetType(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap);
		__declspec(noinline) bool checkDialogCondition(DialogLineData* line, Dialogue* dialog, DialogConditionEnum conditionName, ComparisonEnum compareBy, TalkerEnum who, int val, Character* target, Character* actualConversationTarget);
		__declspec(noinline) bool checkTargetDialoguePackage(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers, const lektor<GameData*>& packages);
		__declspec(noinline) bool checkTargetSpecificCharacter(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, bool ignoreTargetMembers, const lektor<GameData*>& characters);
		__declspec(noinline) bool checkArea(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, const lektor<GameData*>& areas);
		__declspec(noinline) bool checkSpecificTown(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, const lektor<GameData*>& towns);
		__declspec(noinline) bool checkReplacementTown(DialogLineData* line, Dialogue* dialog, Character* target, bool isWordswap, const lektor<GameData*>& towns);
	}
}