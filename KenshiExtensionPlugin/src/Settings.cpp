#include <windows.h>
#include <Settings.h>
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <Debug.h>

#include <kenshi/gui/OptionsWindow.h>
#include <kenshi/gui/DatapanelGUI.h>
#include <kenshi/gui/DataPanelLine.h>

#include <kep/translation.h>
#include <ExternalFunctions.h>

namespace fs = boost::filesystem;

KEP::Settings KEP::settings;

namespace
{
	rapidjson::Document generateDocument(const KEP::Settings& settings)
	{
		rapidjson::Document settingsDocument;
		settingsDocument.SetObject();
		settingsDocument.AddMember("fix_npc_faces", settings._fixShapeKey, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_hair_display", settings._fixHairDisplay, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_portrait", settings._fixPortrait, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_animation_override", settings._fixAnimationOverride, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_spawning_of_unique_characters", settings._fixspawningOfUniqueCharacters, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_unloading_of_unique_characters", settings._fixUnloadingOfUniqueCharacters, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_importing_of_unique_prisoners", settings._fixImportingOfUniquePrisoners, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_special_leader", settings._fixSpecialLeader, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_melee_combat_xp", settings._fixMeleeCombatXP, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_lockpicking_xp", settings._fixLockpickingXP, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_unarmed_damage_bonus", settings._fixUnarmedDamageBonus, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_stat_name_display", settings._fixStatNameDisplay, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_movement_animation", settings._fixMovementAnimation, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_pet_recovery_speed", settings._fixPetRecoverySpeed, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_order_of_hp_bar", settings._fixHPBarOrder, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_the_injury_calculation", settings._fixTheInjuryCalculation, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_the_blood_when_spawning", settings._fixTheBloodWhenSpawning, settingsDocument.GetAllocator());
		settingsDocument.AddMember("change_armor_damage_reduction_order", settings._ChangeArmorDamageReductionOrder, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_global_damage_multiplier", settings._fixGlobalDamageMultiplier, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_item_becoming_sltolen_goods_from_player_bug", settings._fixItemBecomingStolenGoodsFromPlayerBug, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_housemates_inventory_refresh", settings._fixHousemateInventoryRefresh, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_blueprint_texture", settings._fixBlueprintTextures, settingsDocument.GetAllocator());
		settingsDocument.AddMember("enable_player_involvement", settings._enablePlayerInvolvement, settingsDocument.GetAllocator());
		settingsDocument.AddMember("enable_the_navigation_mesh_cache", settings._enableTheNavigationMeshCache, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_town_override", settings._fixTownOverride, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_visible_distant_town", settings._fixVisibleDistantTown, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_building_improvements", settings._fixBuildingImprovements, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_itemtype_limit", settings._fixItemTypeLimit, settingsDocument.GetAllocator());
		settingsDocument.AddMember("enable_notreal_war_campaign", settings._enableNotRealWarCampaign, settingsDocument.GetAllocator());
		settingsDocument.AddMember("sorted_idle_stances", settings._sortedIdleStances, settingsDocument.GetAllocator());
		settingsDocument.AddMember("character_dismissal_function_extension", settings._dismissedCharacterExtension, settingsDocument.GetAllocator());
		settingsDocument.AddMember("furnace_extension", settings._furnaceExtension, settingsDocument.GetAllocator());
		settingsDocument.AddMember("race_equipment_limits_extension", settings._raceEquipmentLimitsExtension, settingsDocument.GetAllocator());
		settingsDocument.AddMember("scythe_path_extension", settings._scythePathExtension, settingsDocument.GetAllocator());
		settingsDocument.AddMember("bulk_limits_extension", settings._bulkLimitsExtension, settingsDocument.GetAllocator());
		settingsDocument.AddMember("bulk_stat_max_male", settings._bulkStatMaxMale, settingsDocument.GetAllocator());
		settingsDocument.AddMember("bulk_stat_min_male", settings._bulkStatMinMale, settingsDocument.GetAllocator());
		settingsDocument.AddMember("bulk_stat_max_female", settings._bulkStatMaxFemale, settingsDocument.GetAllocator());
		settingsDocument.AddMember("bulk_stat_min_female", settings._bulkStatMinFemale, settingsDocument.GetAllocator());
		settingsDocument.AddMember("idle_stance_extension", settings._idleStanceExtension, settingsDocument.GetAllocator());
		settingsDocument.AddMember("animation_skill_type_extension", settings._animationSkillTypeExtension, settingsDocument.GetAllocator());
		settingsDocument.AddMember("portrait_extension", settings._portraitExtension, settingsDocument.GetAllocator());
		settingsDocument.AddMember("save_reputation", settings._saveReputation, settingsDocument.GetAllocator());
		settingsDocument.AddMember("aim_animation_extension", settings._aimAnimationExtension, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_athletics_multiplier", settings._fixAthleticsMultiplier, settingsDocument.GetAllocator());
		settingsDocument.AddMember("animal_armor", settings._animalArmor, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_tech_and_crafting_queue", settings._fixTechAndCraftingQueue, settingsDocument.GetAllocator());
		settingsDocument.AddMember("enable_crash_prevention", settings._enableCrashPrevention, settingsDocument.GetAllocator());
		settingsDocument.AddMember("crafting_item_extension", settings._craftingItemExtension, settingsDocument.GetAllocator());
		settingsDocument.AddMember("weapon_extension", settings._weaponExtension, settingsDocument.GetAllocator());
		settingsDocument.AddMember("dialogue_extension", settings._dialogueExtension, settingsDocument.GetAllocator());
		settingsDocument.AddMember("sorted_new_game_starts", settings._sortedNewGameStarts, settingsDocument.GetAllocator());
		settingsDocument.AddMember("character_extension", settings._characterExtension, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_GetResourceFilePath", settings._fixGetResourceFilePath, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fix_TortureBuilding", settings._fixTortureBuilding, settingsDocument.GetAllocator());

		return settingsDocument;
	}
}

KEP::Settings::Settings()
	: _settingsPath("")
	, _fixShapeKey(true)
	, _fixHairDisplay(true)
	, _fixPortrait(true)
	, _fixAnimationOverride(true)
	, _fixspawningOfUniqueCharacters(true)
	, _fixUnloadingOfUniqueCharacters(true)
	, _fixImportingOfUniquePrisoners(true)
	, _fixSpecialLeader(true)
	, _fixMeleeCombatXP(true)
	, _fixLockpickingXP(true)
	, _fixUnarmedDamageBonus(true)
	, _fixStatNameDisplay(true)
	, _fixMovementAnimation(true)
	, _fixPetRecoverySpeed(true)
	, _fixHPBarOrder(true)
	, _fixTheInjuryCalculation(true)
	, _fixTheBloodWhenSpawning(true)
	, _ChangeArmorDamageReductionOrder(true)
	, _fixGlobalDamageMultiplier(true)
	, _fixItemBecomingStolenGoodsFromPlayerBug(true)
	, _fixHousemateInventoryRefresh(true)
	, _fixBlueprintTextures(true)
	, _enablePlayerInvolvement(true)
	, _enableTheNavigationMeshCache(true)
	, _fixTownOverride(true)
	, _fixVisibleDistantTown(true)
	, _fixBuildingImprovements(true)
	, _fixItemTypeLimit(false)
	, _enableNotRealWarCampaign(false)
	, _dismissedCharacterExtension(true)
	, _raceEquipmentLimitsExtension(true)
	, _furnaceExtension(true)
	, _sortedIdleStances(true)
	, _scythePathExtension(true)
	, _aimAnimationExtension(true)
	, _bulkLimitsExtension(false)
	, _bulkStatMaxMale(500.0f)
	, _bulkStatMinMale(-500.0f)
	, _bulkStatMaxFemale(500.0f)
	, _bulkStatMinFemale(-500.0f)
	, _idleStanceExtension(false)
	, _saveReputation(false)
	, _animationSkillTypeExtension(false)
	, _portraitExtension(0)
	, _fixAthleticsMultiplier(true)
	, _animalArmor(true)
	, _fixTechAndCraftingQueue(false)
	, _enableCrashPrevention(true)
	, _craftingItemExtension(false)
	, _weaponExtension(false)
	, _dialogueExtension(false)
	, _sortedNewGameStarts(false)
	, _xpMod(false)
	, _characterExtension(true)
	, _fixGetResourceFilePath(true)
	, _fixTortureBuilding(true)
{
}

void KEP::Settings::init(const std::string& path)
{
	_settingsPath = path;
}

void KEP::Settings::saveSettings() const
{
	std::ofstream ofs(_settingsPath);
	if (!ofs.is_open())
	{
		DebugLog("save settings fialed");
		return;
	}

	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	auto settingsDocument = generateDocument(*this);
	settingsDocument.Accept(writer);
}

void KEP::Settings::loadSettings()
{
	std::ifstream ifs(_settingsPath);
	if (!ifs.is_open())
		return;

	rapidjson::IStreamWrapper isw(ifs);
	rapidjson::Document settingsDocument;
	settingsDocument.ParseStream(isw);
	if (settingsDocument.HasParseError() || !settingsDocument.IsObject())
		return;

	if (settingsDocument.HasMember("fix_npc_faces"))
		this->_fixShapeKey = settingsDocument["fix_npc_faces"].GetBool();
	if (settingsDocument.HasMember("fix_hair_display"))
		this->_fixHairDisplay = settingsDocument["fix_hair_display"].GetBool();
	if (settingsDocument.HasMember("fix_portrait"))
		this->_fixPortrait = settingsDocument["fix_portrait"].GetBool();
	if (settingsDocument.HasMember("fix_animation_override"))
		this->_fixAnimationOverride = settingsDocument["fix_animation_override"].GetBool();
	if (settingsDocument.HasMember("fix_spawning_of_unique_characters"))
		this->_fixspawningOfUniqueCharacters = settingsDocument["fix_spawning_of_unique_characters"].GetBool();
	if (settingsDocument.HasMember("fix_unloading_of_unique_characters"))
		this->_fixUnloadingOfUniqueCharacters = settingsDocument["fix_unloading_of_unique_characters"].GetBool();
	if (settingsDocument.HasMember("fix_importing_of_unique_prisoners"))
		this->_fixImportingOfUniquePrisoners = settingsDocument["fix_importing_of_unique_prisoners"].GetBool();
	if (settingsDocument.HasMember("fix_special_leader"))
		this->_fixSpecialLeader = settingsDocument["fix_special_leader"].GetBool();
	if (settingsDocument.HasMember("fix_melee_combat_xp"))
		this->_fixMeleeCombatXP = settingsDocument["fix_melee_combat_xp"].GetBool();
	if (settingsDocument.HasMember("fix_lockpicking_xp"))
		this->_fixLockpickingXP = settingsDocument["fix_lockpicking_xp"].GetBool();
	if (settingsDocument.HasMember("fix_unarmed_damage_bonus"))
		this->_fixUnarmedDamageBonus = settingsDocument["fix_unarmed_damage_bonus"].GetBool();
	if (settingsDocument.HasMember("fix_stat_name_display"))
		this->_fixStatNameDisplay = settingsDocument["fix_stat_name_display"].GetBool();
	if (settingsDocument.HasMember("fix_movement_animation"))
		this->_fixMovementAnimation = settingsDocument["fix_movement_animation"].GetBool();
	if (settingsDocument.HasMember("fix_pet_recovery_speed"))
		this->_fixPetRecoverySpeed = settingsDocument["fix_pet_recovery_speed"].GetBool();
	if (settingsDocument.HasMember("fix_order_of_hp_bar"))
		this->_fixHPBarOrder = settingsDocument["fix_order_of_hp_bar"].GetBool();
	if (settingsDocument.HasMember("fix_the_injury_calculation"))
		this->_fixTheInjuryCalculation = settingsDocument["fix_the_injury_calculation"].GetBool();
	if (settingsDocument.HasMember("fix_the_blood_when_spawning"))
		this->_fixTheBloodWhenSpawning = settingsDocument["fix_the_blood_when_spawning"].GetBool();
	if (settingsDocument.HasMember("change_armor_damage_reduction_order"))
		this->_ChangeArmorDamageReductionOrder = settingsDocument["change_armor_damage_reduction_order"].GetBool();
	if (settingsDocument.HasMember("fix_global_damage_multiplier"))
		this->_fixGlobalDamageMultiplier = settingsDocument["fix_global_damage_multiplier"].GetBool();
	if (settingsDocument.HasMember("fix_item_becoming_sltolen_goods_from_player_bug"))
		this->_fixItemBecomingStolenGoodsFromPlayerBug = settingsDocument["fix_item_becoming_sltolen_goods_from_player_bug"].GetBool();
	if (settingsDocument.HasMember("fix_housemates_inventory_refresh"))
		this->_fixHousemateInventoryRefresh = settingsDocument["fix_housemates_inventory_refresh"].GetBool();
	if (settingsDocument.HasMember("fix_blueprint_texture"))
		this->_fixBlueprintTextures = settingsDocument["fix_blueprint_texture"].GetBool();
	if (settingsDocument.HasMember("enable_player_involvement"))
		this->_enablePlayerInvolvement = settingsDocument["enable_player_involvement"].GetBool();
	if (settingsDocument.HasMember("enable_the_navigation_mesh_cache"))
		this->_enableTheNavigationMeshCache = settingsDocument["enable_the_navigation_mesh_cache"].GetBool();
	if (settingsDocument.HasMember("fix_town_override"))
		this->_fixTownOverride = settingsDocument["fix_town_override"].GetBool();
	if (settingsDocument.HasMember("fix_visible_distant_town"))
		this->_fixVisibleDistantTown = settingsDocument["fix_visible_distant_town"].GetBool();
	if (settingsDocument.HasMember("fix_building_improvements"))
		this->_fixBuildingImprovements = settingsDocument["fix_building_improvements"].GetBool();
	if (settingsDocument.HasMember("fix_itemtype_limit"))
		this->_fixItemTypeLimit = settingsDocument["fix_itemtype_limit"].GetBool();
	if (settingsDocument.HasMember("enable_notreal_war_campaign"))
		this->_enableNotRealWarCampaign = settingsDocument["enable_notreal_war_campaign"].GetBool();
	if (settingsDocument.HasMember("sorted_idle_stances"))
		this->_sortedIdleStances = settingsDocument["sorted_idle_stances"].GetBool();
	if (settingsDocument.HasMember("character_dismissal_function_extension"))
		this->_dismissedCharacterExtension = settingsDocument["character_dismissal_function_extension"].GetBool();
	if (settingsDocument.HasMember("furnace_extension"))
		this->_furnaceExtension = settingsDocument["furnace_extension"].GetBool();
	if (settingsDocument.HasMember("race_equipment_limits_extension"))
		this->_raceEquipmentLimitsExtension = settingsDocument["race_equipment_limits_extension"].GetBool();
	if (settingsDocument.HasMember("scythe_path_extension"))
		this->_scythePathExtension = settingsDocument["scythe_path_extension"].GetBool();
	if (settingsDocument.HasMember("bulk_limits_extension"))
		this->_bulkLimitsExtension = settingsDocument["bulk_limits_extension"].GetBool();
	if (settingsDocument.HasMember("bulk_stat_max_male"))
		this->_bulkStatMaxMale = settingsDocument["bulk_stat_max_male"].GetFloat();
	if (settingsDocument.HasMember("bulk_stat_min_male"))
		this->_bulkStatMinMale = settingsDocument["bulk_stat_min_male"].GetFloat();
	if (settingsDocument.HasMember("bulk_stat_max_female"))
		this->_bulkStatMaxFemale = settingsDocument["bulk_stat_max_female"].GetFloat();
	if (settingsDocument.HasMember("bulk_stat_min_female"))
		this->_bulkStatMinFemale = settingsDocument["bulk_stat_min_female"].GetFloat();
	if (settingsDocument.HasMember("idle_stance_extension"))
		this->_idleStanceExtension = settingsDocument["idle_stance_extension"].GetBool();
	if (settingsDocument.HasMember("animation_skill_type_extension"))
		this->_animationSkillTypeExtension = settingsDocument["animation_skill_type_extension"].GetBool();
	if (settingsDocument.HasMember("portrait_extension"))
		this->_portraitExtension = settingsDocument["portrait_extension"].GetInt();
	if (settingsDocument.HasMember("save_reputation"))
		this->_saveReputation = settingsDocument["save_reputation"].GetBool();
	if (settingsDocument.HasMember("aim_animation_extension"))
		this->_aimAnimationExtension = settingsDocument["aim_animation_extension"].GetBool();
	if (settingsDocument.HasMember("fix_athletics_multiplier"))
		this->_fixAthleticsMultiplier = settingsDocument["fix_athletics_multiplier"].GetBool();
	if (settingsDocument.HasMember("animal_armor"))
		this->_animalArmor = settingsDocument["animal_armor"].GetBool();
	if (settingsDocument.HasMember("fix_tech_and_crafting_queue"))
		this->_fixTechAndCraftingQueue = settingsDocument["fix_tech_and_crafting_queue"].GetBool();
	if (settingsDocument.HasMember("enable_crash_prevention"))
		this->_enableCrashPrevention = settingsDocument["enable_crash_prevention"].GetBool();
	if (settingsDocument.HasMember("crafting_item_extension"))
		this->_craftingItemExtension = settingsDocument["crafting_item_extension"].GetBool();
	if (settingsDocument.HasMember("weapon_extension"))
		this->_weaponExtension = settingsDocument["weapon_extension"].GetBool();
	if (settingsDocument.HasMember("dialogue_extension"))
		this->_dialogueExtension = settingsDocument["dialogue_extension"].GetBool();
	if (settingsDocument.HasMember("sorted_new_game_starts"))
		this->_sortedNewGameStarts = settingsDocument["sorted_new_game_starts"].GetBool();
	if (settingsDocument.HasMember("character_extension"))
		this->_characterExtension = settingsDocument["character_extension"].GetBool();
	if (settingsDocument.HasMember("fix_GetResourceFilePath"))
		this->_fixGetResourceFilePath = settingsDocument["fix_GetResourceFilePath"].GetBool();
	if (settingsDocument.HasMember("fix_TortureBuilding"))
		this->_fixTortureBuilding = settingsDocument["fix_TortureBuilding"].GetBool();
}

KEP::Settings::~Settings()
{
}

void KEP::Settings::save() const
{
	this->saveSettings();
}

void KEP::Settings::create(DatapanelGUI* panel, int category, ToolTip* tooltip)
{
	panel->setLine(*externalGlobals->_MainColorCode + KEP::TranslationUtility::gettext("[KEP Bug fixes]"), "", category, false, true);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix NPC faces"), &this->_fixShapeKey, category)
		->setToolTip(KEP::TranslationUtility::gettext("NPC shape keys are applied correctly."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix hair display in the character editor"), &this->_fixHairDisplay, category)
		->setToolTip(KEP::TranslationUtility::gettext("Characters equipped with bandanas and similar items will now display correctly in the Character Editor window."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix portrait"), &this->_fixPortrait, category)
		->setToolTip(KEP::TranslationUtility::gettext("The idle animations added or overwritten by Mods will be reflected in the portrait. Additionally, the positions of the shoulders and neck will be displayed correctly."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix animation override"), &this->_fixAnimationOverride, category)
		->setToolTip(KEP::TranslationUtility::gettext("The animation will be overridden correctly."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix spawning of unique characters"), &this->_fixspawningOfUniqueCharacters, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents the loss of unique characters due to town overrides or squad regeneration."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix unloading of unique characters"), &this->_fixUnloadingOfUniqueCharacters, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents unique characters participating in town raid events from teleporting hometown."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix importing of unique prisoners"), &this->_fixImportingOfUniquePrisoners, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents unique prisoners that have already appeared from disappearing after import."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix the unique leader for the Town Raid event"), &this->_fixSpecialLeader, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents imprisoned unique characters from becoming leaders of town raid events."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix melee combat XP"), &this->_fixMeleeCombatXP, category)
		->setToolTip(KEP::TranslationUtility::gettext("Race bonuses for martial arts and weapon skills apply to melee combat XP. Additionally, SOL for weapon skills etc. functions correctly."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix lockpicking XP"), &this->_fixLockpickingXP, category)
		->setToolTip(KEP::TranslationUtility::gettext("The \"exp gain multiplier\" is applied to lockpicking XP."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix unarmed damage bonus"), &this->_fixUnarmedDamageBonus, category)
		->setToolTip(KEP::TranslationUtility::gettext("The \"Unarmed Damage Bonus\" applies to martial arts skills."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix calculation of athletics effect"), &this->_fixAthleticsMultiplier, category)
		->setToolTip(KEP::TranslationUtility::gettext("The bonus to movement ability from backpacks applies even when wearing armor."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix stat name display"), &this->_fixStatNameDisplay, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents Polearm from failing to display in certain GUIs."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix the prosthetic leg movement animation"), &this->_fixMovementAnimation, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents limping when the maximum HP of the left and right legs differs significantly."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix pet recovery speed"), &this->_fixPetRecoverySpeed, category)
		->setToolTip(KEP::TranslationUtility::gettext("Pet recovery speed prioritizes the bed's effect over the stun effect."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix order of HP bar"), &this->_fixHPBarOrder, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents the HP bars for the chest and stomach from being reversed."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix the injury calculation"), &this->_fixTheInjuryCalculation, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents Hive from becoming bedridden at the base when it takes a very small cut damage."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix the blood when spawning"), &this->_fixTheBloodWhenSpawning, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents characters from spawning with low blood."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix global damage multiplier for Crossbow"), &this->_fixGlobalDamageMultiplier, category)
		->setToolTip(KEP::TranslationUtility::gettext("The global damage multiplier will no longer significantly affect crossbows."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix item becoming stolen goods from player bug"), &this->_fixItemBecomingStolenGoodsFromPlayerBug, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents an issue where taking items out of a backpack placed on the ground halves their selling price."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix housemates inventory refresh"), &this->_fixHousemateInventoryRefresh, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents an issue where Waystation bars, etc., do not refresh their inventory."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix blueprint texture"), &this->_fixBlueprintTextures, category)
		->setToolTip(KEP::TranslationUtility::gettext("Materials can be applied to blueprints."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Enable player involvement"), &this->_enablePlayerInvolvement, category)
		->setToolTip(KEP::TranslationUtility::gettext("World State player involvement functions correctly."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Enable Navigation Mesh cache"), &this->_enableTheNavigationMeshCache, category)
		->setToolTip(KEP::TranslationUtility::gettext("Accelerates zone loading and prevents falling through walls. Kenshi CTD fix patch takes precedence, so option changes will be disabled while it is in use."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix town override"), &this->_fixTownOverride, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents overridden town settings from reverting to their original values."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix display for distant towns"), &this->_fixVisibleDistantTown, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents the town loaded during loading from occasionally failing to switch to the distant mesh."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix research and crafting queue sizes"), &this->_fixTechAndCraftingQueue, category)
		->setToolTip(KEP::TranslationUtility::gettext("Revert queue height to version 1.0.59 or earlier. Additionally, it will now scale according to window size instead of font size. (REQURIES RESTART)"), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Enable duplicate research on building improvements"), &this->_fixBuildingImprovements, category)
		->setToolTip(KEP::TranslationUtility::gettext("The effects of research to increase building output can be applied multiple times."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Change the order of armor damage reduction"), &this->_ChangeArmorDamageReductionOrder, category)
		->setToolTip(KEP::TranslationUtility::gettext("Damage is reduced in order of armor with the highest slash resistance efficiency. Changing settings during play requires saving and reloading."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix peeler machine"), &this->_fixTortureBuilding, category)
		->setToolTip(KEP::TranslationUtility::gettext("Prevents an issue where characters become unable to be released from the peeler machine. (REQURIES RESTART)"), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix crossbow locker"), &this->_fixItemTypeLimit, category)
		->setToolTip(KEP::TranslationUtility::gettext("Item type limits will be enforced."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fix UtilityT::getResourceFilePath"), &this->_fixGetResourceFilePath, category)
		->setToolTip(KEP::TranslationUtility::gettext("Change the search path priority for `UtilityT::getResourceFilePath` to match that of the OGRE resource manager."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Enable crash prevention"), &this->_enableCrashPrevention, category)
		->setToolTip(KEP::TranslationUtility::gettext("Fixes two causes of random crashes. See the documentation for details. (REQURIES RESTART)\nNote: This doesn't prevent all crashes."), tooltip);

	panel->addSpace(category, 1.0f);

	panel->setLine(*externalGlobals->_MainColorCode + KEP::TranslationUtility::gettext("[KEP Features]"), "", category, false, true);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Sort idle stances"), &this->_sortedIdleStances, category)
		->setToolTip(KEP::TranslationUtility::gettext("Sort idle stances by Mod load order and dictionary order of names. (REQURIES RESTART)"), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Sort New Game Starts"), &this->_sortedNewGameStarts, category)
		->setToolTip(KEP::TranslationUtility::gettext("Sort New Game Starts by Mod load order and dictionary order of stringID. (REQURIES RESTART)"), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Enable raid events from factions not displayed in the list"), &this->_enableNotRealWarCampaign, category)
		->setToolTip(KEP::TranslationUtility::gettext("Raid events will occur from factions such as Fogmen and Fishmen. Changing settings during play requires saving and reloading."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Save reputation points"), &this->_saveReputation, category)
		->setToolTip(KEP::TranslationUtility::gettext("Saving reputation points."), tooltip);

	auto panelLine_portraitExtension = panel->setLineDropBox(KEP::TranslationUtility::gettext("Player portrait capacity cap"), category, &this->_portraitExtension, false, 0.4f);
	panelLine_portraitExtension->setToolTip(KEP::TranslationUtility::gettext("Select player portrait capacity caps. Default is 256 people. (REQURIES RESTART)"), tooltip);
	panelLine_portraitExtension->addAValue("256", 0);
	panelLine_portraitExtension->addAValue("1024", 1);
	panelLine_portraitExtension->refresh();

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Extension: Character Dismissal"), &this->_dismissedCharacterExtension, category)
		->setToolTip(KEP::TranslationUtility::gettext("Enable extension function for Character Dismissal."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Extension: Item Furnace"), &this->_furnaceExtension, category)
		->setToolTip(KEP::TranslationUtility::gettext("Enable extension function for Item Furnaces. (REQURIES RESTART)"), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Extension: Race Equipment Limits"), &this->_raceEquipmentLimitsExtension, category)
		->setToolTip(KEP::TranslationUtility::gettext("Enable extension function for Race Equipment Limits. (REQURIES RESTART)"), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Extension: Scythe Path"), &this->_scythePathExtension, category)
		->setToolTip(KEP::TranslationUtility::gettext("Enable extension function for Scythe Path."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Extension: Bulk Limits"), &this->_bulkLimitsExtension, category)
		->setToolTip(KEP::TranslationUtility::gettext("Enable extension function for Bulk Limits."), tooltip);

	auto panelLine_bulkStatMaxMale = panel->setLineSliderEditable(KEP::TranslationUtility::gettext("Max Bulk Stat for Males"), category, true, -2000.0f, 2000.0f, &this->_bulkStatMaxMale);
	panelLine_bulkStatMaxMale->setToolTip(KEP::TranslationUtility::gettext("Sets the max value for stats affecting bulk. Setting both to 20 will lock them at the standard size."), tooltip);
	panelLine_bulkStatMaxMale->setPrecision(0);

	auto panelLine_bulkStatMinMale = panel->setLineSliderEditable(KEP::TranslationUtility::gettext("Min Bulk Stat for Males"), category, true, -2000.0f, 2000.0f, &this->_bulkStatMinMale);
	panelLine_bulkStatMinMale->setToolTip(KEP::TranslationUtility::gettext("Sets the min value for stats affecting bulk. Setting both to 20 will lock them at the standard size."), tooltip);
	panelLine_bulkStatMinMale->setPrecision(0);

	auto panelLine_bulkStatMaxFemale = panel->setLineSliderEditable(KEP::TranslationUtility::gettext("Max Bulk Stat for Females"), category, true, -2000.0f, 2000.0f, &this->_bulkStatMaxFemale);
	panelLine_bulkStatMaxFemale->setToolTip(KEP::TranslationUtility::gettext("Sets the max value for stats affecting bulk. Setting both to 20 will lock them at the standard size."), tooltip);
	panelLine_bulkStatMaxFemale->setPrecision(0);

	auto panelLine_bulkStatMinFemale = panel->setLineSliderEditable(KEP::TranslationUtility::gettext("Min Bulk Stat for Females"), category, true, -2000.0f, 2000.0f, &this->_bulkStatMinFemale);
	panelLine_bulkStatMinFemale->setToolTip(KEP::TranslationUtility::gettext("Sets the min value for stats affecting bulk. Setting both to 20 will lock them at the standard size."), tooltip);
	panelLine_bulkStatMinFemale->setPrecision(0);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Extension: Idle Stance"), &this->_idleStanceExtension, category)
		->setToolTip(KEP::TranslationUtility::gettext("Enable extension function for Idle Stance."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Extension: Animation SkillType"), &this->_animationSkillTypeExtension, category)
		->setToolTip(KEP::TranslationUtility::gettext("Enable extension function for Animation SkillType. (REQURIES RESTART)"), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Extension: Aim Animation"), &this->_aimAnimationExtension, category)
		->setToolTip(KEP::TranslationUtility::gettext("Enable extension function for Aim Animation."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Extension: Animal Armor"), &this->_animalArmor, category)
		->setToolTip(KEP::TranslationUtility::gettext("Enable extension function for Animal Armor. (REQURIES RESTART)"), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Extension: Crafting item"), &this->_craftingItemExtension, category)
		->setToolTip(KEP::TranslationUtility::gettext("Enable extension function for Crafting item. (REQURIES RESTART)"), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Extension: Weapon"), &this->_weaponExtension, category)
		->setToolTip(KEP::TranslationUtility::gettext("Enable extension function for Weapon. (REQURIES RESTART)"), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Extension: Dialogue"), &this->_dialogueExtension, category)
		->setToolTip(KEP::TranslationUtility::gettext("Enable extension function for Dialogue."), tooltip);

	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Extension: Character"), &this->_characterExtension, category)
		->setToolTip(KEP::TranslationUtility::gettext("Enable extension function for Character."), tooltip);
}
