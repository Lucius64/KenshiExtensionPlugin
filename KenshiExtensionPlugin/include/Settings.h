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
		bool _dialogueExtension;
		bool _sortedNewGameStarts;
		bool _xpMod;
		bool _characterExtension;
		bool _fixGetResourceFilePath;
		bool _fixTortureBuilding;
		bool _fixAnimalAge;
		bool _fixIdleAnimation;
	};
}