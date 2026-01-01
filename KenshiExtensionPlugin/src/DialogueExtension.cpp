#include <ogre/OgrePrerequisites.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/GameData.h>
#include <kenshi/Faction.h>
#include <kenshi/Dialogue.h>
#include <kenshi/Character.h>

#include <extern/FactionRelations.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <DialogueExtension.h>


namespace
{
	void (*FactionRelations_saveState_orig)(FactionRelations*, GameData*);
	void (*FactionRelations_loadState_orig)(FactionRelations*, GameData*, bool);
}

void KEP::DialogueExtension::FactionRelations_saveState_hook(FactionRelations* self, GameData* state)
{
	if (settings._saveReputation)
		state->fdata["reputation"] = self->reputation;
	FactionRelations_saveState_orig(self, state);
}

void KEP::DialogueExtension::FactionRelations_loadState_hook(FactionRelations* self, GameData* state, bool isImport)
{
	FactionRelations_loadState_orig(self, state, isImport);
	if (settings._saveReputation)
		self->reputation = state->fdata["reputation"];
}

void KEP::DialogueExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_006B3460, &FactionRelations_saveState_hook, &FactionRelations_saveState_orig))
		ErrorLog("KenshiExtensionPlugin: [save reputation pt1] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_006B4A70, &FactionRelations_loadState_hook, &FactionRelations_loadState_orig))
		ErrorLog("KenshiExtensionPlugin: [save reputation pt2] could not install hook!");
}
