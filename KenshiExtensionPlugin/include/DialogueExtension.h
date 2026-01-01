#pragma once

class FactionRelations;
class GameData;
class Character;
class DialogLineData;
class Dialogue;

namespace KEP
{
	namespace DialogueExtension
	{
		void FactionRelations_saveState_hook(FactionRelations* self, GameData* state);
		void FactionRelations_loadState_hook(FactionRelations* self, GameData* state, bool isImport);
		void init();
	}
}