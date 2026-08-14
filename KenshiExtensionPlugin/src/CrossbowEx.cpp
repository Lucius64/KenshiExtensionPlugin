/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <boost/locale.hpp>

#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreEntity.h>

#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GlobalConstants.h>
#include <kenshi/OptionsHolder.h>
#include <kenshi/GameplayOptions.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/gui/ForgottenGUI.h>
#include <kenshi/Faction.h>
#include <kenshi/Platoon.h>
#include <kenshi/RaceData.h>
#include <kenshi/Character.h>
#include <kenshi/CharStats.h>
#include <kenshi/Gear.h>
#include <kenshi/GunClass.h>
#include <kenshi/AI/AI.h>
#include <kenshi/Animation/AnimationClass.h>
#include <kenshi/Appearance.h>
#include <kenshi/Damages.h>

#include <extern/ConstantTracerT.h>

#include <kep/functions.h>
#include <kep/utility.h>
#include <kep/translation.h>
#include <UtilityFunction.h>
#include <Settings.h>
#include <CrossbowEx.h>

class Wound : public Ogre::GeneralAllocatedObject
{
public:
	AppearanceBase* appearance;
	AttackDirection::Enum attackDirection;
	int _0xc;
	float bleed;
	float currentBleed;
	bool _0x18;
	size_t _0x20;
	Harpoon* harpoon;
	std::string bone;

	Wound(AppearanceBase* _appearance, void* _a2, const std::string& boneName, float bleedMult, AttackDirection::Enum _attackDirection, Harpoon* _harpoon)
		: appearance(_appearance)
		, attackDirection(_attackDirection)
		, bleed(bleedMult)
		, currentBleed(bleedMult)
		, harpoon(_harpoon)
		, bone(boneName)
	{
		appearance->addWound(this);
	}

	~Wound()
	{
		if (appearance != nullptr)
			appearance->removeWound(this);
	}

private:

};

namespace AK
{
	namespace SoundEngine
	{
		unsigned long (*PostEvent)(char const* __ptr64, unsigned __int64, unsigned long, void(__cdecl*)(enum AkCallbackType, struct AkCallbackInfo* __ptr64), void* __ptr64, unsigned long, struct AkExternalSourceInfo* __ptr64, unsigned long);
	}
}

namespace
{
	void (*Crossbow_getTooltipData2_orig)(Crossbow*, Ogre::vector<StringPair>::type&);
	void Crossbow_getTooltipData2_hook(Crossbow* self, Ogre::vector<StringPair>::type& lines)
	{
		Crossbow_getTooltipData2_orig(self, lines);
		if (KEP::settings._showCrafter && !self->crafter.empty())
		{
			auto& mainColour = KEP::GUIColor::getMain();
			auto id_craftedBy = "Crafted by {1}";
			lines.push_back(StringPair("", ""));

			lines.push_back(
				StringPair(
					mainColour + "[" + KEP::TranslationUtility::format_main(boost::locale::format(boost::locale::translate(id_craftedBy)) % self->crafter) + "]"
				)
			);

			auto& secondaryColour = KEP::GUIColor::getSecondary();
			auto id = self->level_0_100 < 70 ? "Crafted by the smith {1} of {2}." : "A masterful weapon crafted by the renowned smith {1} of {2}.";
			lines.push_back(
				StringPair(
					secondaryColour + KEP::TranslationUtility::format_main(boost::locale::format(boost::locale::translate(id)) % self->crafter % ou->player->participant->name)
				)
			);
		}
	}

	boost::shared_mutex mutex;
	Ogre::map<Harpoon*, GameData*>::type rangedWeaponsMap;

	void (*GunClass_shoot_orig)(GunClass*, Character*, RootObject*, StatsEnumerated, const Ogre::Vector3&);
	void GunClass_shoot_hook(GunClass* self, Character* me, RootObject* target, StatsEnumerated stat, const Ogre::Vector3& aimpos)
	{
		if (!KEP::settings._rangedWeaponsEx)
		{
			GunClass_shoot_orig(self, me, target, stat, aimpos);
			return;
		}

		self->setSkillForGUI(me->stats->getStat(stat, false), me->getHandle());
		if (self->drawnStringMesh != nullptr)
			self->drawnStringMesh->setVisible(false);
		if (self->slackStringMesh != nullptr)
			self->slackStringMesh->setVisible(true);

		auto pos = self->getBarrelPos();
		if (self->tracer != nullptr && self->didThisTraceHitOurMountedBuilding(self->tracer->_0x8->hit))
			pos = self->getBarrelPosForTrace(50.0f);

		float heightBonus = 1.0f;
		--self->numShotsCurrent;

		if (-1 < self->numShotsCurrent && self->numShotsCurrent < static_cast<int>(self->ammoObjects.size()))
			self->ammoObjects[self->numShotsCurrent]->setVisible(false);

		if (self->harpoonMaterial.isNull())
			return;

		auto poon = KEP::functions->HarpoonManager_create(KEP::functions->getHarpoonManager(), self->liveAmmoMesh, self->harpoonMaterial->getName());
		if (poon == nullptr)
			return;

		{
			boost::unique_lock<boost::shared_mutex> lock(mutex);
			rangedWeaponsMap[poon] = self->gunData;
		}

		poon->opponent = target->getHandle();
		poon->shooter = me->handle;
		if (target != nullptr)
			heightBonus = self->calculateHeightBonus(pos, target->getPosition());
		heightBonus *= self->calculateWallBonusMult();

		float accuracyDeviation = 0.0f;
		if (1.0f <= self->accuracySkillRequired)
			accuracyDeviation = -self->accuracyDeviationBase * (me->stats->getStat(STAT_PERCEPTION, false) * 0.005f + me->stats->getStat(stat, false) * 0.005f) / (self->accuracySkillRequired * 0.01f) + self->accuracyDeviationBase;

		if (accuracyDeviation < 0.0f)
			accuracyDeviation = 0.0f;
		Ogre::Radian deviation(Ogre::Math::DegreesToRadians(UtilityT::random() * accuracyDeviation));
		auto deviantDir = self->getAimDir(aimpos).randomDeviant(deviation);

		KEP::functions->Harpoon_trace(poon, pos, deviantDir);

		float powerEfficiencyMult = self->getPowerEfficiencyMult();
		poon->damage = UtilityT::random(static_cast<float>(self->getDamageMin()), static_cast<float>(self->getDamageMax())) * powerEfficiencyMult * heightBonus;
		poon->range = powerEfficiencyMult * self->maxRange * heightBonus;
		poon->shotSpeed = self->shotSpeed * 0.8f * powerEfficiencyMult * heightBonus + self->shotSpeed * 0.2f;
		float friendlyFireAvoidance = me->stats->getStat(STAT_FRIENDLY_FIRE, false) * 0.015f;
		if (1.0f < friendlyFireAvoidance)
			friendlyFireAvoidance = 1.0f;
		poon->friendlyFireAvoidance = friendlyFireAvoidance;
		self->getAudioObject();

		AK::SoundEngine::PostEvent("Attack", self->getAudioObject(), 0, nullptr, nullptr, 0, nullptr, 0);
		if (target != nullptr)
			static_cast<Character*>(target)->ai->underRangedAttack(me, true);
	}

	void (*Harpoon_destroy_orig)(Harpoon*);
	void Harpoon_destroy_hook(Harpoon* self)
	{
		Harpoon_destroy_orig(self);
		if (KEP::settings._rangedWeaponsEx)
		{
			boost::unique_lock<boost::shared_mutex> lock(mutex);
			rangedWeaponsMap.erase(self);
		}
	}

	GameData* (*MedicalSystem_addWound_orig)(MedicalSystem*, bool, CutDirection, Damages&, int&, RootObject*, AttackDirection::Enum&, Harpoon*);
	GameData* MedicalSystem_addWound_hook(MedicalSystem* self, bool lowBlow, CutDirection area, Damages& damage, int& material, RootObject* attacker, AttackDirection::Enum& attackDirection, Harpoon* harpoon)
	{
		if (KEP::settings._rangedWeaponsEx && harpoon != nullptr)
		{
			boost::shared_lock<boost::shared_mutex> lock(mutex);
			auto gunData = rangedWeaponsMap[harpoon];
			if (gunData != nullptr)
			{
				auto fdataIt = gunData->fdata.find("bleed mult");
				if (fdataIt != gunData->fdata.end())
					damage.bleedMult = fdataIt->second;
				if (self->me->myRace->robot)
				{
					fdataIt = gunData->fdata.find("robot damage mult");
					if (fdataIt != gunData->fdata.end())
						damage.pierce *= fdataIt->second;
				}
				else if (self->me->isAnimal())
				{
					fdataIt = gunData->fdata.find("animal damage mult");
					if (fdataIt != gunData->fdata.end())
						damage.pierce *= fdataIt->second;
				}
				else if (self->me->isHuman())
				{
					fdataIt = gunData->fdata.find("human damage mult");
					if (fdataIt != gunData->fdata.end())
						damage.pierce *= fdataIt->second;
				}
				auto& raceDamage = gunData->objectReferences["race damage"];
				for (auto iter = raceDamage.begin(); iter != raceDamage.end(); ++iter)
				{
					auto raceData = iter->getPtr(&ou->gamedata);
					if (raceData == self->me->myRace->data)
					{
						damage.pierce *= iter->values.value[0] * 0.01f;
						break;
					}
				}
			}
		}

		if (KEP::settings._fixDamageCalculation < 1)
			return MedicalSystem_addWound_orig(self, lowBlow, area, damage, material, attacker, attackDirection, harpoon);

		FitnessSelector<MedicalSystem::HealthPartStatus*> anatomies;
		for (auto anatomyIt = self->anatomy.begin(); anatomyIt != self->anatomy.end(); ++anatomyIt)
		{
			auto anatomy = *anatomyIt;
			if (-anatomy->maxHealth() < anatomy->flesh && anatomy->getRobotLimbState() != LIMB_STUMP)
			{
				if (!lowBlow || anatomy->whatAmI != MedicalSystem::HealthPartStatus::PART_ARM && anatomy->whatAmI != MedicalSystem::HealthPartStatus::PART_HEAD)
				{
					float chance = anatomy->hitChance;
					if (area != CUT_PIERCED)
						chance *= anatomy->hitChanceMult;
					anatomies.addItem(anatomy, chance);
				}
			}
		}

		MedicalSystem::HealthPartStatus* hitPart = nullptr;
		if (anatomies.size() < 1)
			hitPart = self->anatomy[self->anatomy.size() - 1];
		else
			hitPart = anatomies.chooseAnItem();

		float threshold = 4.0f;
		if (-hitPart->maxHealth() < hitPart->flesh)
		{
			if (hitPart->derivedFleshHealthPercent < 0.0)
			{
				threshold = 1.0f;
				if (2.0f < hitPart->hitChanceMult)
					hitPart->hitChanceMult = 2.0f;
			}
		}
		else
		{
			threshold = -999.0f;
			hitPart->hitChanceMult = 0.0f;
		}
		if (hitPart->hitChanceMult < threshold)
			hitPart->hitChanceMult += 1.0f;

		material = -1;

		float totalCutResistance = 0.0f;
		float totalBluntResistance = 0.0f;
		float totalPierceResistance = 0.0f;
		float totalCutToStun = 0.0f;

		int armourCount = 0;
		for (auto armourIt = self->armourList.begin(); armourIt != self->armourList.end(); ++armourIt)
		{
			auto armour = *armourIt;
			if (!armour->didIHitFlesh(hitPart->data))
			{
				++armourCount;
				float temp = (1.0f - totalCutResistance);
				totalCutResistance += armour->cutResistance * temp;
				totalBluntResistance += armour->bluntResistance * (1.0f - totalBluntResistance);
				totalPierceResistance += armour->pierceResistance;
				totalCutToStun += armour->cutResistance * armour->cutToStun * temp;
				material = armour->materialType;
			}
		}

		if (self->me->getRace()->robot || hitPart->getRobotLimbState() == LIMB_REPLACED)
			material = 3;

		float ap = damage.armourPenetration;
		if (KEP::settings._fixDamageCalculation == 2)
		{
			float threshold = 0.0f;
			float base = 0.9f;
			for (int i = 0; i < armourCount; i++)
			{
				threshold += base;
				base *= 0.1f;
			}
			if (threshold != 0.0f && ap != 0.0f)
			{
				totalCutResistance -= ap * totalCutResistance;
				totalBluntResistance -= ap * totalBluntResistance;
				totalCutToStun -= ap * totalCutToStun;
				if (threshold < totalCutResistance)
				{
					totalCutToStun *= threshold / totalCutResistance;
					totalCutResistance = threshold;
				}
				if (threshold < totalBluntResistance)
					totalBluntResistance = threshold;
			}
		}
		else if (ap != 0.0f)
		{
			totalCutResistance -= ap * totalCutResistance;
			totalBluntResistance -= ap * totalBluntResistance;
			if (0.9f < totalCutResistance)
				totalCutResistance = 0.9f;
			if (0.9f < totalBluntResistance)
				totalBluntResistance = 0.9f;
		}

		damage.extraStun = totalCutToStun * damage.cut;

		float reducedCut = damage.cut * totalCutResistance;
		damage.cut -= reducedCut;

		float reducedBlunt = damage.blunt * totalBluntResistance;
		damage.blunt -= reducedBlunt;

		float pierce = damage.pierce;
		float reducedPierce = totalPierceResistance;
		damage.pierce -= reducedPierce;
		if (damage.pierce < 0.0f)
		{
			reducedPierce = pierce;
			damage.pierce = 0.0f;
		}

		damage.extraStun += (pierce - damage.pierce) * totalCutToStun;

		float toughness = KEP::clamp(self->stats->_toughness * self->stats->ageMult * 0.01f, 0.0f, 1.0f);
		float toughnessResistance = KEP::lerp(toughness, con->DAMAGE_RESISTANCE_MIN, con->DAMAGE_RESISTANCE_MAX);
		damage.cut -= damage.cut * toughnessResistance;
		damage.blunt -= damage.blunt * toughnessResistance;
		damage.pierce -= damage.pierce * toughnessResistance;
		damage.extraStun -= damage.extraStun * toughnessResistance;
		if (damage.cut < 0.0f)
			damage.cut = 0.0f;
		if (damage.blunt < 0.0f)
			damage.blunt = 0.0f;

		damage.multiply(KEP::functions->getGameplayOptions()->globalDamageMultiplier);

		auto pos = attacker->getPosition() - self->me->getPosition();
		pos.normalise();
		if (area == CUT_DOWNWARD)
		{
			pos.x = 0.0f;
			pos.z = 0.0f;
			pos += Ogre::Vector3(pos.z, -1.0f, pos.x);
		}
		else if (area == CUT_LEFT)
		{
			pos.z = -pos.z;
			pos += Ogre::Vector3(pos.z, 0.0f, pos.x);
		}
		else if (area == CUT_RIGHT)
		{
			pos.x = -pos.x;
			pos += Ogre::Vector3(pos.z, 0.0f, pos.x);
		}
		else if (area == CUT_UPWARDS)
		{
			pos.x = 0.0f;
			pos.z = 0.0f;
			pos += Ogre::Vector3(pos.z, 1.0f, pos.x);
		}

		self->applyDamage(hitPart, damage, false, true, pos * (damage.blunt * 1000.0f));
		auto& boneName = MedicalSystem::getBoneNameForBodypart(hitPart->data);

		float bleed = damage.cut < damage.pierce ? damage.pierce : damage.cut;
		attackDirection = AttackDirection::FRONT;
		if (self->me->animation->isActivated)
		{
			auto body = self->me->animation->appearance->body;
			auto myYaw = body->getParentSceneNode()->getOrientation().getYaw();
			auto attackerYaw = (harpoon == nullptr ? static_cast<Character*>(attacker)->animation->appearance->body->getParentSceneNode() : harpoon->node)->getOrientation().getYaw();

			auto diff = Ogre::Math::Abs(myYaw.valueRadians() - attackerYaw.valueRadians());
			if (diff < Ogre::Math::HALF_PI || Ogre::Math::PI * 1.5f < diff)
				attackDirection = AttackDirection::BACK;
		}

		self->wounds.push_back(new Wound(self->animation->appearance, nullptr, boneName, bleed * con->BLEED_RATE_MULTIPLIER * damage.bleedMult, attackDirection, harpoon));

		if (0 < options->damageFloaters && self->me->getPosition().squaredDistance(ou->getCameraPos()) < 16000000.0f)
		{
			std::string text;
			if (options->damageFloaters == 2)
			{
				text.append(hitPart->data->name);
				text.append(": ");
				if (damage.pierce < 1.0f)
				{
					text.append(KEP::TranslationUtility::format_main(boost::locale::format("{1,num}") % static_cast<int>(damage.cut)));
					if (0.0f < reducedCut)
					{
						text.append(" (-");
						text.append(KEP::TranslationUtility::format_main(boost::locale::format("{1,num}") % static_cast<int>(reducedCut)));
						text.append(")");
					}
				}
				else
				{
					text.append(KEP::TranslationUtility::format_main(boost::locale::format("{1,num}") % static_cast<int>(damage.pierce + damage.cut)));
					if (0.0f < reducedPierce + reducedCut)
					{
						text.append(" (-");
						text.append(KEP::TranslationUtility::format_main(boost::locale::format("{1,num}") % static_cast<int>(reducedPierce + reducedCut)));
						text.append(")");
					}
				}

				if (1.0f <= damage.blunt + damage.extraStun)
				{
					text.append(" + ");
					text.append(KEP::TranslationUtility::format_main(boost::locale::format("{1,num}") % static_cast<int>(damage.blunt + damage.extraStun)));
					if (0.0f < reducedBlunt)
					{
						text.append(" (-");
						text.append(KEP::TranslationUtility::format_main(boost::locale::format("{1,num}") % static_cast<int>(reducedBlunt)));
						text.append(")");
					}
				}
			}
			else
			{
				int totalDamage = static_cast<int>(damage.total());
				text.append(KEP::TranslationUtility::format_main(boost::locale::format("{1,num}") % totalDamage));
				if (0.0f < reducedPierce + reducedCut + reducedBlunt)
				{
					text.append(" (-");
					text.append(KEP::TranslationUtility::format_main(boost::locale::format("{1,num}") % static_cast<int>(reducedPierce + reducedCut + reducedBlunt)));
					text.append(")");
				}
			}
			if (gui->visible)
			{
				auto platoon = static_cast<ActivePlatoon*>(ou->player->getCurrentActivePlatoon());
				if (platoon != nullptr && self->me->isOnScreen)
				{
					ScreenLabel::LabelSize textSize = options->damageFloaters == 1 ? ScreenLabel::LS_MEDIUM : ScreenLabel::LS_SMALL;
					auto label = gui->createScreenLabel(text, MyGUI::Colour(0.9f, 0.9f, 0.9f, 0.9f), textSize, ScreenLabel::RS_SLOW);
					label->setTracking(self->me->getHandle(), Ogre::Vector3(0.0f, 15.0f, 0.0f));
					if (static_cast<Character*>(attacker)->isPlayerCharacter() && KEP::functions->CharacterMemory_getCharacterMemoryTag(platoon->_myMemory, self->me, ST_TEMPORARY_ENEMY))
						label->setColor(MyGUI::Colour(0.11f, 0.6f, 0.11f, 0.9f));
					if (self->me->isPlayerCharacter() || KEP::functions->CharacterMemory_getCharacterMemoryTag(platoon->_myMemory, self->me, ST_TEMPORARY_ALLY))
						label->setColor(MyGUI::Colour(1.0f, 0.31f, 0.31f, 0.9f));
				}
			}
		}

		self->splatBlood(damage, area, attackDirection);
		return hitPart->data;
	}
}

void KEP::CrossbowEx::init()
{
	*(uintptr_t*)&AK::SoundEngine::PostEvent = reinterpret_cast<uintptr_t>(GetProcAddress(nullptr, "?PostEvent@SoundEngine@AK@@YAKPEBD_KKP6AXW4AkCallbackType@@PEAUAkCallbackInfo@@@ZPEAXKPEAUAkExternalSourceInfo@@K@Z"));

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&Crossbow::_NV_getTooltipData2), &Crossbow_getTooltipData2_hook, &Crossbow_getTooltipData2_orig))
		ErrorLog("[Crossbow::getTooltipData2] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&GunClass::shoot), &GunClass_shoot_hook, &GunClass_shoot_orig))
		ErrorLog("[GunClass::shoot] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KEP::functions->Harpoon_destroy, &Harpoon_destroy_hook, &Harpoon_destroy_orig))
		ErrorLog("[Harpoon::destroy] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&MedicalSystem::addWound), &MedicalSystem_addWound_hook, &MedicalSystem_addWound_orig))
		ErrorLog("[MedicalSystem::addWound] could not install hook!");
}
