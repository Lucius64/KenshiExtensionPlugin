/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#pragma once
#include <kenshi/Tasker.h>
#include <kenshi/util/TimeOfDay.h>

class Task_MakeTargetStandStill : public Tasker
{
public:
	virtual ~Task_MakeTargetStandStill();
	virtual void standStill(Character*, Character*);
	virtual void runAction(CharBody* _a1) override;
};

class Item;

class Task_FirstAid : public Task_MakeTargetStandStill
{
public:
	virtual ~Task_FirstAid();
	virtual void startAction(CharBody* _a1) override;
	virtual void runAction(CharBody* _a1) override;
	virtual void endAction(CharBody* _a1) override;
	virtual bool applyFirstAid(Task_FirstAid*, Character*, float, Character*, bool);
	virtual Item* getItem(ItemFunction, Character*);

	bool robotRepair;
	Item* item;
};

class Task_FirstAidRig : public Task_FirstAid
{
public:
	virtual ~Task_FirstAidRig();
	virtual void runAction(CharBody* _a1) override;
};

class ChaseMode
{
public:
	TimeOfDay _0x8;
	bool _0x10;

	virtual bool vfunc0x0(Character*, Character*);
	virtual bool vfunc0x8(Character*, Character*);
};

class Task_MeleeAttack : public Tasker, public ChaseMode
{
public:
	virtual ~Task_MeleeAttack();
	virtual void startAction(CharBody* _a1) override;
	virtual void runAction(CharBody* _a1) override;
	virtual void endAction(CharBody* _a1) override;
	virtual bool taskSaysItsFinished(Character* me) const override;
	virtual void vfunc0x28(Character*);
};

class Unprovoked_Mixin
{
public:
	virtual void vfunc0x0(Character*);
	virtual void vfunc0x8(Character*, Character*);

	bool unprovoked;
};

class Task_FocusedMeleeAttack : public Task_MeleeAttack, public Unprovoked_Mixin
{
public:
	virtual ~Task_FocusedMeleeAttack();
	virtual void startAction(CharBody* _a1) override;
	virtual void runAction(CharBody* _a1) override;
	virtual void endAction(CharBody* _a1) override;
};

class Task_RangedAttack : public Tasker, public Unprovoked_Mixin
{
public:
	virtual ~Task_RangedAttack();
	virtual void startAction(CharBody* _a1) override;
	virtual void runAction(CharBody* _a1) override;
	virtual void endAction(CharBody* _a1) override;

	hand _0x88;
};
