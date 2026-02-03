#include <ogre/OgrePrerequisites.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/util/UtilityT.h>
#include <kenshi/Globals.h>
#include <kenshi/GlobalConstants.h>
#include <kenshi/GameWorld.h>
#include <kenshi/OptionsHolder.h>
#include <kenshi/MedicalSystem.h>
#include <kenshi/Faction.h>
#include <kenshi/Character.h>
#include <kenshi/CharStats.h>
#include <kenshi/GameData.h>
#include <kenshi/RaceData.h>
#include <kenshi/Building.h>
#include <kenshi/ModInfo.h>
#include <kenshi/Logger.h>

#include <extern/AnimationClass.h>
#include <extern/Renderer.h>
#include <extern/UseableStuff.h>

#include <UtilityFunction.h>
#include <ExternalFunctions.h>
#include <Settings.h>
#include <bugfix/MedicalFix.h>


namespace
{
	void (*MedicalSystem_updateDamageState_orig)(MedicalSystem*);
	void (*MedicalSystem_medicalUpdate_orig)(MedicalSystem*, float);
	void (*MedicalSystem_init_orig)(MedicalSystem*, AnimationClass*, Character*, GameData*, CharStats*);
	void (*MedicalSystem_precalculateFirstAidNeedScore_orig)(MedicalSystem*);
	void (*CharStats_init_orig)(CharStats*, GameData*, MedicalSystem*, Character*);


	float getMaxHp(MedicalSystem::HealthPartStatus* slef)
	{
		return slef->_maxHealth * slef->age * slef->HPMult - slef->wearDamage;
	}

	float calcHealedHpPercentage(MedicalSystem::HealthPartStatus* slef)
	{
		return 100.0f / getMaxHp(slef) * (slef->bandaging + slef->flesh);
	}

	float calcRemainingHpPercentage(MedicalSystem::HealthPartStatus* slef)
	{
		return 100.0f / getMaxHp(slef) * (slef->flesh - slef->fleshStun);
	}
}

void KEP::MedicalFix::MedicalSystem_updateDamageState_hook(MedicalSystem* self)
{
	MedicalSystem_updateDamageState_orig(self);

	if (settings._fixMovementAnimation)
		self->animation->setLegDamage(calcRemainingHpPercentage(self->leftLeg), calcRemainingHpPercentage(self->rightLeg));
}

void KEP::MedicalFix::MedicalSystem_medicalUpdate_hook(MedicalSystem* self, float frameTime)
{
	if (!settings._fixPetRecoverySpeed)
	{
		MedicalSystem_medicalUpdate_orig(self, frameTime);
		return;
	}

	if (0.0f < self->currentBleedRate
		&& options->blood
		&& self->me->isStandingOnBuilding().type == NULL_ITEM
		&& self->me->getWaterLevel() == WaterState::NO_WATER)
	{
		float speed;
		if (self->me->_isBeingCarried)
		{
			speed = 0.0f;
			Character* carryingCharacter = self->me->carryingObject.getCharacter();
			if (carryingCharacter != nullptr)
				speed = carryingCharacter->getMovementSpeed();
		}
		else
		{
			speed = self->me->getMovementSpeed();
		}

		if (speed != 0.0f || !self->me->isDown())
		{
			if (!self->me->_isBeingCarried && self->me->isDown())
			{
				auto pos = self->me->getPosition();
				float diffZ = self->lastBloodPosition.z - pos.z;
				float diffX = self->lastBloodPosition.x - pos.x;
				if (16.0f < diffZ * diffZ + diffX * diffX)
				{
					self->lastBloodPosition = pos;
					auto race = self->me->getRace();
					Ogre::ColourValue bloodColour(race->bloodColour.r, race->bloodColour.g, race->bloodColour.b, self->currentBleedRate);

					if (1.0f < bloodColour.a)
						bloodColour.a = 1.0f;
					else if (bloodColour.a < 0.0f)
						bloodColour.a = 0.0f;

					auto pos2 = self->me->getMovementDirection();
					externalFunctions->FUN_008DBF80(externalGlobals->_CLASS_021322B0->renderer->terrainDecalsManager, 3, self->lastBloodPosition, pos2, 0.2f, bloodColour, 99.0f);
				}
			}
			else if (0.01f < self->currentBleedRate)
			{
				float bleed = UtilityT::random() * self->currentBleedRate * frameTime * 10.0f + self->lastBloodPosition.y;
				self->lastBloodPosition.y = bleed;

				if (1.0f < bleed)
				{
					auto pos = self->me->getPosition();
					float diffZ = self->lastBloodPosition.z - pos.z;
					float diffX = self->lastBloodPosition.x - pos.x;
					if (1.0f < diffZ * diffZ + diffX * diffX)
					{
						float randz = UtilityT::random();
						float randx = UtilityT::random();
						Ogre::Vector3 pos2(randx - 0.5f, 0.0f, randz - 0.5f);

						float f1 = self->currentBleedRate * 0.1f;
						if (0.5f < f1)
							f1 = 0.5f;

						auto race = self->me->getRace();

						externalFunctions->FUN_008DBF80(externalGlobals->_CLASS_021322B0->renderer->terrainDecalsManager, 2, pos, pos2, f1, race->bloodColour, 99.0f);

						self->lastBloodPosition = pos;
					}

					self->lastBloodPosition.y = 0.0f;
				}
			}
		}
		else
		{
			auto pos = self->me->getPosition();
			auto race = self->me->getRace();
			externalFunctions->FUN_008DC0D0(externalGlobals->_CLASS_021322B0->renderer->terrainDecalsManager, self->me, pos, self->currentBleedRate, race->bloodColour);

			self->lastBloodPosition = pos;
		}
	}

	if (!self->dead)
	{

		self->bloodlossUpdate(frameTime);
		if (self->knockoutTimer <= 0.0f)
		{
			self->knockoutTimer = 0.0f;
		}
		else
		{
			self->knockoutTimer -= frameTime;
		}

		float maxBlood = self->getMaxBlood();
		if (self->blood < -maxBlood || self->hunger < 0.0f)
		{
			self->dead = true;
			self->me->declareDead();
		}

		float degeneration = lerp(self->stats->_toughness * self->stats->ageMult * 0.01f, con->DEGENERATION_MULT_1, con->DEGENERATION_MULT_99);
		float healRateWear = 0.0f;
		float healRateRobot = 1.0f;
		float healRateFlesh = 1.0f;

		if (self->me->getFaction()->isPlayer != nullptr && self->me->isAnimal() != nullptr && self->me->isUnconcious())
		{
			healRateRobot = 2.0f;
			healRateFlesh = 2.0f;
		}

		auto inWhat = self->me->inSomething;
		if (inWhat == IN_BED)
		{
			auto building = self->me->inWhat.getBuilding();
			if (building != nullptr)
			{
				degeneration = 0.0f;

				auto bf = building->getSpecialFunction();
				if (bf == BF_BED)
				{
					healRateFlesh = building->getProductionMult();
				}
				else if (bf == BF_SKELETON_BED)
				{
					healRateRobot = building->getProductionMult() * building->getUseableStuff()->calculateEfficiencyMult();
					healRateWear = healRateRobot;
				}
			}
		}
		else if (inWhat == IN_PRISON)
		{
			healRateFlesh = 2.0f;
		}

		float healRateRace = self->me->myRace->healRate;
		float charaScl = self->me->getAge();

		for (auto it = self->anatomy.begin(); it != self->anatomy.end(); ++it)
		{
			bool isAlive = (*it)->update(frameTime, healRateRace * healRateFlesh, healRateRace * healRateRobot, degeneration, charaScl, healRateWear);
			if (!isAlive)
			{
				self->dead = true;
				self->me->declareDead();
				return;
			}
		}

		float currentBlood = self->blood;
		if (currentBlood < maxBlood)
		{
			float bloodRecoveryRate = con->BLOOD_RECOVERY_RATE;
			if (inWhat == IN_BED)
				bloodRecoveryRate *= con->BODYPART_HEAL_RATE_MULT_RESTING;

			if (self->currentBleedRate <= 0.0f)
				self->blood = bloodRecoveryRate * frameTime * healRateRace + currentBlood;

			if (self->blood > 25.0f)
				self->bloodlossTrauma = false;

		}
	}
}

void KEP::MedicalFix::MedicalSystem_init_hook(MedicalSystem* self, AnimationClass* animsys, Character* c, GameData* race, CharStats* _stats)
{
	if (!settings._fixHPBarOrder)
	{
		MedicalSystem_init_orig(self, animsys, c, race, _stats);
		return;
	}

	static std::array<uint8_t, 4> BodyPartsSortKeys = { 0xa, 0x1e, 0x14, 0x0 };

	self->lastPeriodicUpdate = ou->getTimeStamp_inGameHours().time;
	self->crippled = false;
	self->dead = false;
	self->stats = _stats;
	self->animation = animsys;
	self->me = c;

	self->notifyRaceChange(race);

	Ogre::vector<std::pair<uint8_t, MedicalSystem::HealthPartStatus*>>::type healthPartsList;

	for (auto iter = self->status.begin(); iter != self->status.end(); ++iter)
	{
		auto sortKey = iter->second.whatAmI < 4 ? BodyPartsSortKeys[iter->second.whatAmI] : 0x0;
		auto collapsePartEnum = iter->second.data->idata["collapse part"];
		if (collapsePartEnum == 0x2 || collapsePartEnum == 0x10)
			sortKey++;
		else if (collapsePartEnum == 0x1 && (iter->second.data->stringID == "101-gamedata.quack") || iter->second.data->bdata["is chest"])
			sortKey--;

		healthPartsList.push_back(std::make_pair(sortKey, &iter->second));
	}

	std::sort(healthPartsList.begin(), healthPartsList.end());

	for (auto iter = healthPartsList.begin(); iter != healthPartsList.end(); ++iter)
	{
		self->anatomy.push_back(iter->second);

		if (iter->second->whatAmI == MedicalSystem::HealthPartStatus::PART_LEG)
		{
			if (iter->second->side == SIDE_LEFT)
				self->leftLeg = iter->second;
			else
				self->rightLeg = iter->second;
		}
		else if (iter->second->whatAmI == MedicalSystem::HealthPartStatus::PART_ARM)
		{
			if (iter->second->side == SIDE_LEFT)
				self->leftArm = iter->second;
			else
				self->rightArm = iter->second;
		}
	}

	if (self->leftLeg == nullptr || self->rightLeg == nullptr || self->leftArm == nullptr || self->rightArm == nullptr)
	{
		auto myRace = self->me->getRace();
		Logger::logMessage("Mod Error: Race " + myRace->data->name + " from mod " + ou->activeMods[myRace->data->createdIndex]->name + " is missing body parts", Logger::Fatal);
	}

	auto myRace = self->me->getRace();
	self->blood = lerp(self->me->getAge(), myRace->originalBloodMin, myRace->originalBloodMax);
	if (myRace->robot)
		self->hunger = 3.0f;
}

void KEP::MedicalFix::MedicalSystem_precalculateFirstAidNeedScore_hook(MedicalSystem* self)
{
	if (!settings._fixTheInjuryCalculation)
	{
		MedicalSystem_precalculateFirstAidNeedScore_orig(self);
		return;
	}

	self->worstDamage = 100.0f;
	self->restedState = 100.0f;
	self->partBestArm = -1.0f;
	self->partWorstTorso = 100.0f;
	self->partHead = 0.0f;

	auto race = self->me->getRace();
	float worstFleshFatalDamage = 100.0f;
	float worstRobotFatalDamage = 100.0f;
	float worstRobotDamage = 100.0f;

	for (auto iter = self->anatomy.begin(); iter != self->anatomy.end(); ++iter)
	{
		auto healedHpPercentage = calcHealedHpPercentage(*iter);
		if (!race->robot && (*iter)->robotLimb == nullptr)
		{
			if (healedHpPercentage < worstFleshFatalDamage && (*iter)->fatal)
				worstFleshFatalDamage = healedHpPercentage;

			if (healedHpPercentage < self->worstDamage)
				self->worstDamage = healedHpPercentage;

			auto remainingHpPercentage = calcRemainingHpPercentage(*iter);
			if (remainingHpPercentage < self->restedState)
				self->restedState = remainingHpPercentage;
		}
		else
		{
			if (healedHpPercentage < worstRobotFatalDamage && (*iter)->fatal)
				worstRobotFatalDamage = healedHpPercentage;

			if (healedHpPercentage < worstRobotDamage)
				worstRobotDamage = healedHpPercentage;
		}

		if ((*iter)->whatAmI == MedicalSystem::HealthPartStatus::PART_ARM && self->partBestArm < (*iter)->derivedFleshHealthPercent)
			self->partBestArm = (*iter)->derivedFleshHealthPercent;

		if ((*iter)->whatAmI == MedicalSystem::HealthPartStatus::PART_HEAD)
			self->partHead = (*iter)->derivedFleshHealthPercent;

		if ((*iter)->whatAmI == MedicalSystem::HealthPartStatus::PART_TORSO && (*iter)->derivedFleshHealthPercent < self->partWorstTorso)
			self->partWorstTorso = (*iter)->derivedFleshHealthPercent;
	}

	if (self->me->isAnimal() != nullptr)
		self->partBestArm = 1.0f;

	self->restedState *= 0.01f;
	worstFleshFatalDamage *= 0.01f;
	self->worstDamage *= 0.01f;
	worstRobotFatalDamage *= 0.01f;
	worstRobotDamage *= 0.01f;

	if (0.999f < self->restedState)
		self->restedState = 1.0f;

	if (0.999f < worstFleshFatalDamage)
		worstFleshFatalDamage = 1.0f;

	if (0.999f < self->worstDamage)
		self->worstDamage = 1.0f;

	if (0.999f < worstRobotFatalDamage)
		worstRobotFatalDamage = 1.0f;

	if (0.999f < worstRobotDamage)
		worstRobotDamage = 1.0f;

	self->needsFirstAidScoreTotal_fleshy = -0.1f;
	self->needsFirstAidScoreTotal_robot = -0.1f;

	float needsFirstAidScore_fleshFatal = 2.0f - (worstFleshFatalDamage + 1.0f);
	float needsFirstAidScore_flesh = (2.0f - (self->worstDamage + 1.0f)) * 0.5f;
	if (needsFirstAidScore_flesh < needsFirstAidScore_fleshFatal)
		needsFirstAidScore_flesh = needsFirstAidScore_fleshFatal;

	float needsFirstAidScore_robotFatal = 2.0f - (worstRobotFatalDamage + 1.0f);
	float needsFirstAidScore_robot = (2.0f - (worstRobotDamage + 1.0f)) * 0.5f;
	if (needsFirstAidScore_robot < needsFirstAidScore_robotFatal)
		needsFirstAidScore_robot = needsFirstAidScore_robotFatal;

	float bleed = self->currentBleedRate - self->extraBloodLossFromBodyparts * 1.5f;
	if (bleed < 0.0f)
		bleed = 0.0f;

	float needsFirstAidScore_bleed = (2.0f - (self->blood / self->getMaxBlood() + 1.0f)) * bleed * 10.0f;

	if (!race->robot && needsFirstAidScore_flesh < needsFirstAidScore_bleed && 0.2f < needsFirstAidScore_bleed)
		needsFirstAidScore_flesh = needsFirstAidScore_bleed;

	self->needsFirstAidScoreTotal_fleshy = needsFirstAidScore_flesh;

	if (race->robot && needsFirstAidScore_robot < needsFirstAidScore_bleed && 0.2f < needsFirstAidScore_bleed)
		needsFirstAidScore_robot = needsFirstAidScore_bleed;

	self->needsFirstAidScoreTotal_robot = needsFirstAidScore_robot;
}

void KEP::MedicalFix::CharStats_init_hook(CharStats* self, GameData* data, MedicalSystem* _med, Character* charact)
{
	CharStats_init_orig(self, data, _med, charact);

	if (settings._fixTheBloodWhenSpawning)
		self->medical->blood = self->medical->getMaxBlood();
}

void KEP::MedicalFix::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&MedicalSystem::updateDamageState), &MedicalSystem_updateDamageState_hook, &MedicalSystem_updateDamageState_orig))
		ErrorLog("KenshiExtensionPlugin: [leg damage animation] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&MedicalSystem::medicalUpdate), &MedicalSystem_medicalUpdate_hook, &MedicalSystem_medicalUpdate_orig))
		ErrorLog("KenshiExtensionPlugin: [recovery hp] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&MedicalSystem::init), &MedicalSystem_init_hook, &MedicalSystem_init_orig))
		ErrorLog("KenshiExtensionPlugin: [order health parts] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&MedicalSystem::precalculateFirstAidNeedScore), &MedicalSystem_precalculateFirstAidNeedScore_hook, &MedicalSystem_precalculateFirstAidNeedScore_orig))
		ErrorLog("KenshiExtensionPlugin: [calc injury rate] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharStats::_NV_init), &CharStats_init_hook, &CharStats_init_orig))
		ErrorLog("KenshiExtensionPlugin: [init max blood] could not install hook!");
}
