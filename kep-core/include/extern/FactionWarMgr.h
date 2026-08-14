/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#pragma once
#include <kenshi/Campaign.h>

// KenshiLibでは未定義
class StartInvasionData : public CampaignData
{
public:
	virtual CampaignInstance* createCampaign(int _a1, Faction* _a2, const hand& _a3, const hand& _a4) override;
};

// KenshiLibでは未定義
class GatherAndMove : public CampaignInstance
{
public:
	Ogre::Vector3 _0xe0;
	TimeOfDay _0xf0;
	std::deque<Ogre::Vector3> _0xf8;
	float _0x128;
	int _0x12c;

	virtual ~GatherAndMove();
	virtual void periodicUpdate() override;
	virtual UnloadedPlatoonJob getUnloadedPlatoonBehavior(Platoon* who) override;
	virtual TargetInfo getAITarget(Platoon* who) override;
	virtual Ogre::Vector3 getFormationPosition(Platoon* who) override;
	virtual Ogre::Vector3 getDestination(Platoon* who) override;
	virtual bool isSuccessful() override;
};
