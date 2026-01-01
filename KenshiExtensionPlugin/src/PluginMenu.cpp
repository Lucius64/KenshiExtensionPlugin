#include <boost/locale.hpp>

#include <mygui/MyGUI.h>
#include <mygui/MyGUI_Button.h>
#include <mygui/MyGUI_Window.h>
#include <mygui/MyGUI_TabControl.h>
#include <mygui/MyGUI_TabItem.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <extern/OptionsWindow.h>
#include <extern/DatapanelGUI.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <PluginMenu.h>


namespace
{
	void (*OptionsWindow_FUN_003F0120_orig)(OptionsWindow*);
	void (*OptionsWindow_FUN_003EC950_orig)(OptionsWindow*);
}

void KEP::PluginMenu::OptionsWindow_FUN_003F0120_hook(OptionsWindow* self)
{
	// 以下は使用済みのタブID
	// Tab ID: Genral = 0x1
	// Tab ID: Gameplay = 0x22
	// Tab ID: Graphics = 0x17
	// Tab ID: Audio = 0x18
	// Tab ID: Controls = 0x19
	// Tab ID: Mods = 0x0

	OptionsWindow_FUN_003F0120_orig(self);

	auto pluginOptionTab = self->optionsTab->addItem(boost::locale::gettext("Plugins"));
	auto pluginOptionPanel = externalFunctions->FUN_0073F4B0(externalGlobals->_KenshiGUI, "plugin_options", pluginOptionTab, true);

	const int tabID = 0x30;

	pluginOptionPanel->vfunc0xc0(tabID);
	pluginOptionPanel->vfunc0xe0(25.0f);
	
	externalFunctions->FUN_006FDE20(pluginOptionPanel, *externalGlobals->_MainColorCode + boost::locale::gettext("[Kenshi Extension Plugin Settings]"), "", tabID, false, true);

	auto panelLine_fixShapeKey = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix NPC faces"), settings._fixShapeKey, tabID);
	panelLine_fixShapeKey->setTooltip(boost::locale::gettext("NPC shape keys are applied correctly."), self->tooltip);

	auto panelLine_fixHairDisplayInTheCharacterEditor = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix hair display in the character editor"), settings._fixHairDisplay, tabID);
	panelLine_fixHairDisplayInTheCharacterEditor->setTooltip(boost::locale::gettext("Characters equipped with bandanas and similar items will now display correctly in the Character Editor window."), self->tooltip);

	auto panelLine_fixPortrait = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix portrait"), settings._fixPortrait, tabID);
	panelLine_fixPortrait->setTooltip(boost::locale::gettext("The idle animations added or overwritten by Mods will be reflected in the portrait. Additionally, the positions of the shoulders and neck will be displayed correctly."), self->tooltip);

	auto panelLine_fixAnimationFiles = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix animation override"), settings._fixAnimationOverride, tabID);
	panelLine_fixAnimationFiles->setTooltip(boost::locale::gettext("The animation will be overridden correctly."), self->tooltip);

	auto panelLine_fixspawningOfUniqueCharacters = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix spawning of unique characters"), settings._fixspawningOfUniqueCharacters, tabID);
	panelLine_fixspawningOfUniqueCharacters->setTooltip(boost::locale::gettext("Prevents the loss of unique characters due to town overrides or squad regeneration."), self->tooltip);

	auto panelLine_fixUnloadingOfUniqueCharacters = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix unloading of unique characters"), settings._fixUnloadingOfUniqueCharacters, tabID);
	panelLine_fixUnloadingOfUniqueCharacters->setTooltip(boost::locale::gettext("Prevents unique characters participating in town raid events from teleporting hometown."), self->tooltip);

	auto panelLine_fixImportingOfUniquePrisoners = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix importing of unique prisoners"), settings._fixImportingOfUniquePrisoners, tabID);
	panelLine_fixImportingOfUniquePrisoners->setTooltip(boost::locale::gettext("Prevents unique prisoners that have already appeared from disappearing after import."), self->tooltip);

	auto panelLine_fixSpecialLeader = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix the unique leader for the Town Raid event"), settings._fixSpecialLeader, tabID);
	panelLine_fixSpecialLeader->setTooltip(boost::locale::gettext("Prevents imprisoned unique characters from becoming leaders of town raid events."), self->tooltip);

	auto panelLine_fixMeleeCombatXP = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix melee combat XP"), settings._fixMeleeCombatXP, tabID);
	panelLine_fixMeleeCombatXP->setTooltip(boost::locale::gettext("Race bonuses for martial arts and weapon skills apply to melee combat XP. Additionally, SOL for weapon skills etc. functions correctly."), self->tooltip);

	auto panelLine_fixLockpickingXP = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix lockpicking XP"), settings._fixLockpickingXP, tabID);
	panelLine_fixLockpickingXP->setTooltip(boost::locale::gettext("The \"exp gain multiplier\" is applied to lockpicking XP."), self->tooltip);

	auto panelLine_fixUnarmedDamageBonus = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix unarmed damage bonus"), settings._fixUnarmedDamageBonus, tabID);
	panelLine_fixUnarmedDamageBonus->setTooltip(boost::locale::gettext("The \"Unarmed Damage Bonus\" applies to martial arts skills."), self->tooltip);

	auto panelLine_fixStatNameDisplay = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix stat name display"), settings._fixStatNameDisplay, tabID);
	panelLine_fixStatNameDisplay->setTooltip(boost::locale::gettext("Prevents Polearm from failing to display in certain GUIs."), self->tooltip);

	auto panelLine_fixTheProstheticLegMovementAnimation = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix the prosthetic leg movement animation"), settings._fixMovementAnimation, tabID);
	panelLine_fixTheProstheticLegMovementAnimation->setTooltip(boost::locale::gettext("Prevents limping when the maximum HP of the left and right legs differs significantly."), self->tooltip);

	auto panelLine_fixPetRecoverySpeed = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix pet recovery speed"), settings._fixPetRecoverySpeed, tabID);
	panelLine_fixPetRecoverySpeed->setTooltip(boost::locale::gettext("Pet recovery speed prioritizes the bed's effect over the stun effect."), self->tooltip);

	auto panelLine_fixHPBarOrder = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix order of HP bar"), settings._fixHPBarOrder, tabID);
	panelLine_fixHPBarOrder->setTooltip(boost::locale::gettext("Prevents the HP bars for the chest and stomach from being reversed."), self->tooltip);

	auto panelLine_fixTheInjuryCalculation = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix the injury calculation"), settings._fixTheInjuryCalculation, tabID);
	panelLine_fixTheInjuryCalculation->setTooltip(boost::locale::gettext("Prevents Hive from becoming bedridden at the base when it takes a very small cut damage."), self->tooltip);

	auto panelLine_fixTheBloodWhenSpawning = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix the blood when spawning"), settings._fixTheBloodWhenSpawning, tabID);
	panelLine_fixTheBloodWhenSpawning->setTooltip(boost::locale::gettext("Prevents characters from spawning with low blood."), self->tooltip);

	auto panelLine_ChangeArmorDamageReductionOrder = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Change the order of armor damage reduction"), settings._ChangeArmorDamageReductionOrder, tabID);
	panelLine_ChangeArmorDamageReductionOrder->setTooltip(boost::locale::gettext("Damage is reduced in order of armor with the highest slash resistance efficiency. Changing settings during play requires saving and reloading."), self->tooltip);

	auto panelLine_fixGlobalDamageMultiplierForCrossbow = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix global damage multiplier for Crossbow"), settings._fixGlobalDamageMultiplier, tabID);
	panelLine_fixGlobalDamageMultiplierForCrossbow->setTooltip(boost::locale::gettext("The global damage multiplier will no longer significantly affect crossbows."), self->tooltip);

	auto panelLine_fixItemBecomingStolenGoodsFromPlayerBug = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix item becoming sltolen goods from player bug"), settings._fixItemBecomingStolenGoodsFromPlayerBug, tabID);
	panelLine_fixItemBecomingStolenGoodsFromPlayerBug->setTooltip(boost::locale::gettext("Prevents an issue where taking items out of a backpack placed on the ground halves their selling price."), self->tooltip);

	auto panelLine_fixHousemateInventoryRefresh = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix housemates inventory refresh"), settings._fixHousemateInventoryRefresh, tabID);
	panelLine_fixHousemateInventoryRefresh->setTooltip(boost::locale::gettext("Prevents an issue where Waystation bars, etc., do not refresh their inventory."), self->tooltip);

	auto panelLine_fixBlueprintTextures = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix blueprint texture"), settings._fixBlueprintTextures, tabID);
	panelLine_fixBlueprintTextures->setTooltip(boost::locale::gettext("Materials can be applied to blueprints."), self->tooltip);

	auto panelLine_enablePlayerInvolvement = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Enable player involvement"), settings._enablePlayerInvolvement, tabID);
	panelLine_enablePlayerInvolvement->setTooltip(boost::locale::gettext("World State player involvement functions correctly."), self->tooltip);

	auto panelLine_enableTheNavigationMeshCache = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Enable Navigation Mesh cache"), settings._enableTheNavigationMeshCache, tabID);
	panelLine_enableTheNavigationMeshCache->setTooltip(boost::locale::gettext("Accelerates zone loading and prevents falling through walls. Kenshi CTD fix patch takes precedence, so option changes will be disabled while it is in use."), self->tooltip);

	auto panelLine_fixTownOverride = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix town override"), settings._fixTownOverride, tabID);
	panelLine_fixTownOverride->setTooltip(boost::locale::gettext("Prevents overridden town settings from reverting to their original values."), self->tooltip);

	auto panelLine_fixVisibleDistantTown = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix display for distant towns"), settings._fixVisibleDistantTown, tabID);
	panelLine_fixVisibleDistantTown->setTooltip(boost::locale::gettext("Prevents the town loaded during loading from occasionally failing to switch to the distant mesh."), self->tooltip);

	auto panelLine_fixBuildingImprovements = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Enable duplicate research on building improvements"), settings._fixBuildingImprovements, tabID);
	panelLine_fixBuildingImprovements->setTooltip(boost::locale::gettext("The effects of research to increase building output can be applied multiple times."), self->tooltip);

	auto panelLine_fixItemTypeLimit = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Fix crossbow locker"), settings._fixItemTypeLimit, tabID);
	panelLine_fixItemTypeLimit->setTooltip(boost::locale::gettext("Item type limits will be enforced."), self->tooltip);

	auto panelLine_enableNotRealWarCampaign = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Enable raid events from factions not displayed in the list"), settings._enableNotRealWarCampaign, tabID);
	panelLine_enableNotRealWarCampaign->setTooltip(boost::locale::gettext("Raid events will occur from factions such as Fogmen and Fishmen. Changing settings during play requires saving and reloading."), self->tooltip);

	auto panelLine_sortedIdleStances = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Sort idle stances"), settings._sortedIdleStances, tabID);
	panelLine_sortedIdleStances->setTooltip(boost::locale::gettext("Sort idle stances by Mod load order and dictionary order of names. (REQURIES RESTART)"), self->tooltip);

	auto panelLine_dismissedCharacterExtension = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Extension: Character Dismissal"), settings._dismissedCharacterExtension, tabID);
	panelLine_dismissedCharacterExtension->setTooltip(boost::locale::gettext("Enable extension function for Character Dismissal."), self->tooltip);

	auto panelLine_furnaceExtension = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Extension: Item Furnace"), settings._furnaceExtension, tabID);
	panelLine_furnaceExtension->setTooltip(boost::locale::gettext("Enable extension function for Item Furnaces. (REQURIES RESTART)"), self->tooltip);

	auto panelLine_raceLimiterExtension = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Extension: Race Equipment Limits"), settings._raceEquipmentLimitsExtension, tabID);
	panelLine_raceLimiterExtension->setTooltip(boost::locale::gettext("Enable extension function for Race Equipment Limits. (REQURIES RESTART)"), self->tooltip);

	auto panelLine_scythePathExtension = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Extension: Scythe Path"), settings._scythePathExtension, tabID);
	panelLine_scythePathExtension->setTooltip(boost::locale::gettext("Enable extension function for Scythe Path."), self->tooltip);

	auto panelLine_bulkLimitsExtension = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Extension: Bulk Limits"), settings._bulkLimitsExtension, tabID);
	panelLine_bulkLimitsExtension->setTooltip(boost::locale::gettext("Enable extension function for Bulk Limits."), self->tooltip);

	auto panelLine_bulkStatMaxMale = externalFunctions->FUN_006FE880(pluginOptionPanel, boost::locale::gettext("Max Bulk Stat for Males"), tabID, true, -2000.0f, 2000.0f, settings._bulkStatMaxMale);
	panelLine_bulkStatMaxMale->setTooltip(boost::locale::gettext("Sets the max value for stats affecting bulk."), self->tooltip);
	externalFunctions->FUN_006F57E0(panelLine_bulkStatMaxMale, 0);
	
	auto panelLine_bulkStatMinMale = externalFunctions->FUN_006FE880(pluginOptionPanel, boost::locale::gettext("Min Bulk Stat for Males"), tabID, true, -2000.0f, 2000.0f, settings._bulkStatMinMale);
	panelLine_bulkStatMinMale->setTooltip(boost::locale::gettext("Sets the min value for stats affecting bulk."), self->tooltip);
	externalFunctions->FUN_006F57E0(panelLine_bulkStatMinMale, 0);
	
	auto panelLine_bulkStatMaxFemale = externalFunctions->FUN_006FE880(pluginOptionPanel, boost::locale::gettext("Max Bulk Stat for Females"), tabID, true, -2000.0f, 2000.0f, settings._bulkStatMaxFemale);
	panelLine_bulkStatMaxFemale->setTooltip(boost::locale::gettext("Sets the max value for stats affecting bulk."), self->tooltip);
	externalFunctions->FUN_006F57E0(panelLine_bulkStatMaxFemale, 0);
	
	auto panelLine_bulkStatMinFemale = externalFunctions->FUN_006FE880(pluginOptionPanel, boost::locale::gettext("Min Bulk Stat for Females"), tabID, true, -2000.0f, 2000.0f, settings._bulkStatMinFemale);
	panelLine_bulkStatMinFemale->setTooltip(boost::locale::gettext("Sets the min value for stats affecting bulk."), self->tooltip);
	externalFunctions->FUN_006F57E0(panelLine_bulkStatMinFemale, 0);

	auto panelLine_idleStanceExtension = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Extension: Idle Stance"), settings._idleStanceExtension, tabID);
	panelLine_idleStanceExtension->setTooltip(boost::locale::gettext("Enable extension function for Idle Stance."), self->tooltip);

	auto panelLine_animationSkillTypeExtension = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Extension: Animation SkillType"), settings._animationSkillTypeExtension, tabID);
	panelLine_animationSkillTypeExtension->setTooltip(boost::locale::gettext("Enable extension function for Animation SkillType. (REQURIES RESTART)"), self->tooltip);

	auto panelLine_aimAnimationExtension = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Extension: Aim Animation"), settings._aimAnimationExtension, tabID);
	panelLine_aimAnimationExtension->setTooltip(boost::locale::gettext("Enable extension function for Aim Animation."), self->tooltip);

	auto panelLine_portraitExtension = externalFunctions->FUN_006FE2D0(pluginOptionPanel, boost::locale::gettext("Player portrait capacity cap"), tabID, settings._portraitExtension, false, 0.4f);
	panelLine_portraitExtension->setTooltip(boost::locale::gettext("Select player portrait capacity caps. Default is 256 people. (REQURIES RESTART)"), self->tooltip);
	externalFunctions->FUN_006F6340(panelLine_portraitExtension, "256", 0);
	externalFunctions->FUN_006F6340(panelLine_portraitExtension, "1024", 1);
	panelLine_portraitExtension->chenge();

	auto panelLine_saveReputation = externalFunctions->FUN_006FE210(pluginOptionPanel, boost::locale::gettext("Save reputation points"), settings._saveReputation, tabID);
	panelLine_saveReputation->setTooltip(boost::locale::gettext("Saving reputation points."), self->tooltip);

	pluginOptionTab->setVisible(false);
	self->optionsTab->setItemData(pluginOptionTab, pluginOptionPanel);
}

void KEP::PluginMenu::OptionsWindow_FUN_003EC950_hook(OptionsWindow* self)
{
	OptionsWindow_FUN_003EC950_orig(self);
	settings.saveSettings();
}

void KEP::PluginMenu::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_003F0120, OptionsWindow_FUN_003F0120_hook, &OptionsWindow_FUN_003F0120_orig))
		ErrorLog("KenshiExtensionPlugin: [GUI] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_003EC950, OptionsWindow_FUN_003EC950_hook, &OptionsWindow_FUN_003EC950_orig))
		ErrorLog("KenshiExtensionPlugin: [GUI] Could not add hook!");
}
