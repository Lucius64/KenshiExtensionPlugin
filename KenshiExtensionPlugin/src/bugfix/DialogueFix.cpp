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
#include <kenshi/Town.h>
#include <kenshi/Building/UseableStuff.h>
#include <kenshi/Character.h>
#include <kenshi/CharStats.h>
#include <kenshi/CharMovement.h>
#include <kenshi/CharBody.h>
#include <kenshi/RaceData.h>
#include <kenshi/AI/AI.h>
#include <kenshi/AI/AITaskSystem.h>
#include <kenshi/AI/Blackboard.h>
#include <kenshi/Animation/AnimationClass.h>
#include <kenshi/Item.h>
#include <extern/Task.h>

#include <kep/functions.h>
#include <Settings.h>
#include <EnumExtended.h>
#include <bugfix/DialogueFix.h>

namespace
{
	bool _needFirstAidRobot(Character* who)
	{
		if (!who->medical.dead)
		{
			auto myRace = who->getRace();
			auto& anatomy = who->medical.anatomy;
			for (auto iter = anatomy.begin(); iter != anatomy.end(); ++iter)
			{
				auto healthParts = *iter;
				if (healthParts->bandaging < 100000.0f && healthParts->bandaging + healthParts->flesh < healthParts->maxHealth() && (myRace->robot || healthParts->robotLimb != nullptr))
					return true;
			}
		}
		return false;
	}

	void (*Task_FirstAid_runAction_orig)(Task_FirstAid*, CharBody*);
	void Task_FirstAid_runAction_hook(Task_FirstAid* self, CharBody* body)
	{
		if (!KEP::settings._fixFirstAidDialogueEvent)
		{
			Task_FirstAid_runAction_orig(self, body);
			return;
		}

		if (self->subject.type == NULL_ITEM)
		{
			body->ai->taskSystemAI->taskImpossible();
			return;
		}

		auto target = self->subject.getCharacter();
		if (target == nullptr)
			return;

		auto me = body->getCharacter();

		if (target->myRace->robot)
			self->robotRepair = true;

		ItemFunction itemFunction = ITEM_FIRSTAID;
		me->currentSkillUsing = STAT_MEDIC;
		if (self->robotRepair)
		{
			itemFunction = ITEM_ROBOTREPAIR;
			me->currentSkillUsing = STAT_ROBOTICS;
		}
		self->standStill(me, target);

		if (me->isPlayerCharacter() || target->inSomething != IN_PRISON && !me->isMyFactionsSlave(target))
		{
			ItemFunction medikitType = itemFunction;
			if (self->robotRepair && self->item != nullptr)
			{
				medikitType = ITEM_ROBOTREPAIR;
				if (self->item->itemFunction != ITEM_FIRSTAID)
					medikitType = itemFunction;
			}

			KEP::functions->Task_FirstAid_updateItem(self, medikitType, me, target);
			if (self->item == nullptr)
			{
				if (!self->robotRepair)
				{
					self->robotRepair = true;
					return;
				}
				body->ai->taskSystemAI->_notifyBodyTaskComplete();
				body->stats->currentItemMaximumJuryRig = 0.0f;
				return;
			}
		}

		auto tempItem = self->item;
		bool temp = self->robotRepair;
		bool completed = self->applyFirstAid(self, target, body->frameTIME, me, itemFunction == ITEM_ROBOTREPAIR || self->robotRepair);
		if (completed || temp != self->robotRepair && !_needFirstAidRobot(target))
		{
			body->ai->taskSystemAI->_notifyBodyTaskComplete();
			if (target != me)
			{
				if (!me->isPlayerCharacter() && target->inSomething == IN_PRISON && me->isAlly(target, true))
				{
					auto cage = target->inWhat.getBuilding();
					if (cage != nullptr)
						target->setPrisonMode(false, cage->getUseableStuff());
				}

				me->sendDialogEvent(target, EV_HEALING_OTHER_FINISHED);
				if (target->inSomething != IN_PRISON)
					target->sendDialogEvent(me, EV_BEING_HEALED_FINISHED);
			}
		}
		else if(temp == self->robotRepair && tempItem != nullptr && self->item == nullptr && self->getItem(itemFunction, me) == nullptr)
		{
			me->sendDialogEvent(target, EV_FIRSTAID_KIT_EMPTY);
		}

		me->animation->playAction(me->animation->getAnimation_Medic(me->inWhat), 1.0f, 0.0f, false);
		if (target->isWithThePlayer())
			me->_isEngagedWithAPlayer = true;
	}

	void (*Task_FirstAidRig_runAction_orig)(Task_FirstAidRig*, CharBody*);
	void Task_FirstAidRig_runAction_hook(Task_FirstAidRig* self, CharBody* body)
	{
		if (!KEP::settings._fixFirstAidDialogueEvent)
		{
			Task_FirstAidRig_runAction_orig(self, body);
			return;
		}

		if (self->subject.type == NULL_ITEM)
		{
			body->ai->taskSystemAI->taskImpossible();
			return;
		}

		auto target = self->subject.getCharacter();
		if (target == nullptr)
			return;

		auto me = body->getCharacter();

		auto blackboard = body->ai->getBlackboard();
		if (blackboard != nullptr)
			blackboard->addRequest(me->getHandle(), self->subject, STAND_STILL, hand(0, 0, NULL_ITEM, 0, 0), TP_URGENT, 1.4f);

		body->movement->lookatPosition(target->getPosition());

		if (self->item == nullptr)
		{
			auto item = self->getItem(ITEM_MEDRIGGING, me);
			self->item = item;
			if (KEP::functions->Task_FirstAid_removeItemAutoDestroy(item, body))
			{
				self->item = nullptr;
				item = self->getItem(ITEM_MEDRIGGING, me);
				self->item = item;
				if (item == nullptr)
					me->sendDialogEvent(target, EV_FIRSTAID_KIT_EMPTY);
			}

			body->stats->currentItemMaximumJuryRig = 0.0f;
		}

		if (self->item == nullptr)
		{
			body->ai->taskSystemAI->_notifyBodyTaskComplete();
			body->stats->currentItemMaximumJuryRig = 0.0f;
			return;
		}

		body->stats->currentItemMaximumJuryRig = self->item->quality;
		if (self->item->itemFunction == ITEM_MEDRIGGING)
		{
			bool completed = target->medical.applyRigging(body->stats->medic, self->item, body->frameTIME);
			if (completed)
			{
				body->ai->taskSystemAI->_notifyBodyTaskComplete();
				if (target != me)
				{
					me->sendDialogEvent(target, EV_HEALING_OTHER_FINISHED);
					if (target->inSomething != IN_PRISON)
						target->sendDialogEvent(me, EV_BEING_HEALED_FINISHED);
				}
			}

			if (KEP::functions->Task_FirstAid_removeItemAutoDestroy(self->item, body))
			{
				self->item = nullptr;
				if (!completed && self->getItem(ITEM_MEDRIGGING, me) == nullptr)
					me->sendDialogEvent(target, EV_FIRSTAID_KIT_EMPTY);
			}
		}
		body->stats->xpFirstAid(target, body->frameTIME, STAT_MEDIC);
		me->animation->playAction(me->animation->getAnimation_Medic(me->inWhat), 1.0f, 0.0f, false);
	}
}

void KEP::DialogueFix::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KEP::functions->Task_FirstAid_runAction, &Task_FirstAid_runAction_hook, &Task_FirstAid_runAction_orig))
		ErrorLog("[Task_FirstAid::runAction] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KEP::functions->Task_FirstAidRig_runAction, &Task_FirstAidRig_runAction_hook, &Task_FirstAidRig_runAction_orig))
		ErrorLog("[Task_FirstAidRig::runAction] could not install hook!");
}
