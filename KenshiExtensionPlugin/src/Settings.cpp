#include <windows.h>
#include <Settings.h>
#include <boost/filesystem.hpp>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <Debug.h>

namespace fs = boost::filesystem;

KEP::Settings KEP::settings;
std::string KEP::myDirectory;

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
		DebugLog("Kenshi Extension Plugin: save settings fialed");
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
}
