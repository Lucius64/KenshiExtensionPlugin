/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <ogre/OgreStringConverter.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>
#include <kenshi/Globals.h>
#include <kenshi/Enums.h>
#include <kenshi/Faction.h>
#include <kenshi/FactionRelations.h>
#include <kenshi/Platoon.h>
#include <kenshi/Town.h>
#include <kenshi/Building/UseableStuff.h>
#include <kenshi/Character.h>
#include <kenshi/CharStats.h>
#include <kenshi/CharMovement.h>
#include <kenshi/CharBody.h>
#include <kenshi/Dialogue.h>
#include <kenshi/SensoryData.h>
#include <kenshi/StateBroadcastData.h>
#include <kenshi/RaceData.h>
#include <kenshi/AI/AI.h>
#include <kenshi/AI/AITaskSystem.h>
#include <kenshi/AI/AIPackage.h>
#include <kenshi/AI/Blackboard.h>
#include <kenshi/Animation/AnimationClass.h>
#include <kenshi/Item.h>
#include <extern/Task.h>

#include <kep/functions.h>
#include <Settings.h>
#include <EnumExtended.h>
#include <DialogueEventEx.h>

namespace
{
	bool (*MedicalSystem_gettingWindyface_orig)(MedicalSystem*, float);
	bool MedicalSystem_gettingWindyface_hook(MedicalSystem* self, float amount)
	{
		bool result = MedicalSystem_gettingWindyface_orig(self, amount);
		if (KEP::settings._dialogueEventEx && result)
			self->me->sendDialogEvent(self->me, EV_WINDY);
		return result;
	}

	void (*Task_MeleeAttack_startAction_orig)(Task_MeleeAttack*, CharBody*);
	void Task_MeleeAttack_startAction_hook(Task_MeleeAttack* self, CharBody* body)
	{
		Task_MeleeAttack_startAction_orig(self, body);
		if (KEP::settings._dialogueEventEx)
		{
			auto target = self->subject.getCharacter();
			if (target == nullptr)
				return;

			auto me = body->getCharacter();
			me->sendDialogEvent(target, EV_LAUNCH_ATTACK);
		}
	}

	void (*Task_FocusedMeleeAttack_startAction_orig)(Task_FocusedMeleeAttack*, CharBody*);
	void Task_FocusedMeleeAttack_startAction_hook(Task_FocusedMeleeAttack* self, CharBody* body)
	{
		Task_FocusedMeleeAttack_startAction_orig(self, body);
		if (KEP::settings._dialogueEventEx)
		{
			auto target = self->subject.getCharacter();
			if (target == nullptr)
				return;

			auto me = body->getCharacter();
			me->sendDialogEvent(target, EV_LAUNCH_ATTACK);
		}
	}

	void (*Task_RangedAttack_startAction_orig)(Task_RangedAttack*, CharBody*);
	void Task_RangedAttack_startAction_hook(Task_RangedAttack* self, CharBody* body)
	{
		Task_RangedAttack_startAction_orig(self, body);
		if (KEP::settings._dialogueEventEx)
		{
			auto target = self->subject.getCharacter();
			if (target == nullptr)
				return;

			auto me = body->getCharacter();
			me->sendDialogEvent(target, EV_LAUNCH_ATTACK);
		}
	}

	void (*SensoryData_assessNeutral_orig)(SensoryData*, Character*, bool);
	void SensoryData_assessNeutral_hook(SensoryData* self, Character* who, bool inDepth)
	{
		SensoryData_assessNeutral_orig(self, who, inDepth);
		if (KEP::settings._dialogueEventEx)
		{
			if (who == nullptr
				|| who == self->me
				|| self->me->isAnimal() != nullptr && !self->me->dialogue->hasDialogueEvent(EV_I_SEE_NEUTRAL_SQUAD))
				return;

			if (who->isDestroyed())
				return;

			if (self->me->getPerceptionMult() <= 0.0f)
				return;

			if (!self->me->isOnScreen && !who->isWithThePlayer() && !who->isOnScreen)
				return;

			if (self->me->inSomething == IN_PRISON)
				return;

			if (who->isUnconcious())
			{
				if (!self->me->isInCombatMode(true, true))
					self->me->sendDialogEvent(who, EV_I_SEE_RAGDOLL);
				return;
			}
		}
	}
}

void KEP::DialogueEventEx::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&MedicalSystem::gettingWindyface), &MedicalSystem_gettingWindyface_hook, &MedicalSystem_gettingWindyface_orig))
		ErrorLog("[MedicalSystem::gettingWindyface] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KEP::functions->Task_MeleeAttack_startAction, &Task_MeleeAttack_startAction_hook, &Task_MeleeAttack_startAction_orig))
		ErrorLog("[Task_MeleeAttack::startAction] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KEP::functions->Task_FocusedMeleeAttack_startAction, &Task_FocusedMeleeAttack_startAction_hook, &Task_FocusedMeleeAttack_startAction_orig))
		ErrorLog("[Task_FocusedMeleeAttack::startAction] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KEP::functions->Task_RangedAttack_startAction, &Task_RangedAttack_startAction_hook, &Task_RangedAttack_startAction_orig))
		ErrorLog("[Task_RangedAttack::startAction] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&SensoryData::assessNeutral), &SensoryData_assessNeutral_hook, &SensoryData_assessNeutral_orig))
		ErrorLog("[SensoryData::assessNeutral] could not install hook!");
}
