#pragma once

class GameData;
class Character;
class CharStats;
class MedicalSystem;
class AnimationClass;

namespace KEP
{
	namespace MedicalFix
	{
		void MedicalSystem_updateDamageState_hook(MedicalSystem* self);
		void MedicalSystem_medicalUpdate_hook(MedicalSystem* self, float frameTime);
		void MedicalSystem_init_hook(MedicalSystem* self, AnimationClass* animsys, Character* c, GameData* race, CharStats* _stats);
		void MedicalSystem_precalculateFirstAidNeedScore_hook(MedicalSystem* self);
		void CharStats_init_hook(CharStats* self, GameData* data, MedicalSystem* _med, Character* charact);
		void init();
	}
}