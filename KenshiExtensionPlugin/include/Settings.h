/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#pragma once
#include <string>
#include <kep/config_manager.h>

namespace KEP
{
	class Settings;

	extern Settings settings;

	class  Settings : public IPluginOption
	{
	public:
		Settings();
		void init(const std::string& path);
		void saveSettings() const;
		void loadSettings();
		virtual ~Settings();
		virtual void save() const;
		virtual void create(DatapanelGUI* panel, int category, ToolTip* tooltip);

	private:
		std::string _settingsPath;

	public:
		bool _fixShapeKey;
		bool _fixHairDisplay;
		bool _fixPortrait;
		bool _fixAnimationOverride;
		bool _fixspawningOfUniqueCharacters;
		bool _fixUnloadingOfUniqueCharacters;
		bool _fixImportingOfUniquePrisoners;
		bool _fixSpecialLeader;
		bool _fixMeleeCombatXP;
		bool _fixLockpickingXP;
		bool _fixUnarmedDamageBonus;
		bool _fixStatNameDisplay;
		bool _fixMovementAnimation;
		bool _fixPetRecoverySpeed;
		bool _fixHPBarOrder;
		bool _fixTheInjuryCalculation;
		bool _fixTheBloodWhenSpawning;
		bool _ChangeArmorDamageReductionOrder;
		bool _fixGlobalDamageMultiplier;
		bool _fixItemBecomingStolenGoodsFromPlayerBug;
		bool _fixHousemateInventoryRefresh;
		bool _fixBlueprintTextures;
		bool _enablePlayerInvolvement;
		bool _enableTheNavigationMeshCache;
		bool _fixTownOverride;
		bool _fixVisibleDistantTown;
		bool _fixBuildingImprovements;
		bool _fixItemTypeLimit;
		bool _enableNotRealWarCampaign;
		bool _dismissedCharacterExtension;
		bool _raceEquipmentLimitsExtension;
		bool _furnaceExtension;
		bool _sortedIdleStances;
		bool _scythePathExtension;
		bool _bulkLimitsExtension;
		float _bulkStatMaxMale;
		float _bulkStatMinMale;
		float _bulkStatMaxFemale;
		float _bulkStatMinFemale;
		bool _idleStanceExtension;
		bool _saveReputation;
		bool _animationEx;
		int _portraitExtension;
		bool _fixAthleticsMultiplier;
		bool _animalArmor;
		bool _fixTechAndCraftingQueue;
		bool _enableCrashPrevention;
		bool _craftingItemExtension;
		bool _weaponExtension;
		bool _showCrafter;
		bool _sortArmorCoverage;
		bool _armorEx;
		bool _animalDialoguePackage;
		bool _sortedNewGameStarts;
		bool _xpMod;
		bool _fixNpcMoney;
		bool _extendInitInventory;
		bool _keepCharacterType;
		bool _fixGetResourceFilePath;
		bool _fixTortureBuilding;
		bool _fixAnimalAge;
		bool _fixIdleAnimation;
		bool _researchEx;
		bool _combatTechniquesEx;
		bool _rangedWeaponsEx;
		bool _shoppingEx;
		bool _fixStealingCheck;
		bool _importingNegativeMoney;
		bool _combatEx;
		bool _fixUnarmedBlockChance;
		bool _skillRequirementsForBlockCombatTechniques;
		bool _combatSkillCatAnimOverride;
		bool _fixCombatAnimation;
		bool _specificTargetCampaign;
		bool _wordSwapEx;
		bool _dialogueConditionEx;
		bool _dialogueActionEx;
		bool _dialogueEventEx;
		bool _fixFirstAidDialogueEvent;
		bool _fixRobotLimbItemSerialise;
		bool _fixToughnessXpBonus;
		bool _fixDexterityXP;
		int _fixDamageCalculation;
		int _getEquippedArmour;
	};
}