#include <boost/filesystem.hpp>

#include <ogre/OgreLogManager.h>
#include <ogre/OgreMeshManager.h>
#include <ogre/OgrePose.h>
#include <ogre/OgreStringConverter.h>
#include <ogre/OgreEntity.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/Character.h>
#include <kenshi/AppearanceManager.h>
#include <kenshi/Appearance.h>
#include <kenshi/Animation/AnimationClass.h>
#include <kenshi/gui/CharacterEditWindow.h>

#include <extern/AppearanceBase.h>

#include <UtilityFunction.h>
#include <ExternalFunctions.h>
#include <Settings.h>
#include <bugfix/AppearanceFix.h>

#define DEV_MODE

namespace fs = boost::filesystem;

namespace
{
	struct AnimationDataLess
	{
		bool operator()(const AnimationData* a, const AnimationData* b) const
		{
			if (a->_rawData->createdIndex != b->_rawData->createdIndex)
				return a->_rawData->createdIndex < b->_rawData->createdIndex;

			std::string s1 = a->dataName;
			std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
			std::string s2 = b->dataName;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			return s1 < s2;
		}
	};

	Ogre::Entity* (*AppearanceManager_getMorphedBodyMesh_orig)(AppearanceManager*, GameData*, Ogre::Entity*, bool, float, bool);
	Ogre::Entity* AppearanceManager_getMorphedBodyMesh_hook(AppearanceManager* self, GameData* appearanceData, Ogre::Entity* entity, bool alwaysUnique, float ugliness, bool shaved)
	{
		if (!KEP::settings._fixShapeKey)
			return AppearanceManager_getMorphedBodyMesh_orig(self, appearanceData, entity, alwaysUnique, ugliness, shaved);

		bool existMesh = false;
		auto& pMesh = entity->getMesh();
		auto& meshMgr = Ogre::MeshManager::getSingleton();
		auto& poseList = pMesh->getPoseList();

		size_t numPose = poseList.size();
		if (numPose == 0) // メッシュにシェイプキーがない場合
			return entity;

		std::string morphName = "";

		if (alwaysUnique) // プレイヤーの場合
		{
			auto numPlayerMorph = KEP::externalGlobals->_playerMorphCounter;
			*numPlayerMorph += 1;
			morphName.append(pMesh->getName());
			morphName.append("_morph_");
			morphName.append(KEP::getHexString(*numPlayerMorph, 0));

			existMesh = meshMgr.resourceExists(morphName);
		}
		else
		{
			if (shaved)
			{
				shaved = false;
				for (size_t i = 0; i < numPose; i++)
				{
					if (poseList[i]->getName() == "bone_horns_top_short")
					{
						shaved = true;
						break;
					}
				}
			}

			auto iter = appearanceData->idata.find("morph index");
			if (iter == appearanceData->idata.end()) // 容姿が固定の場合
			{
				uint32_t poseHash = 0;
				bool hasInfluence = false;

				for (size_t i = 0; i < poseList.size(); i++)
				{
					auto& poseName = poseList[i]->getName();
					float poseInfluence = appearanceData->fdata[poseName];

					if (!hasInfluence && poseInfluence != 0.0f)
						hasInfluence = true;

					if (9.223372e+18 < poseInfluence)
						poseInfluence -= static_cast<float>(9.223372e+18);

					//Bugfix
					boost::hash_detail::hash_combine_impl(poseHash, static_cast<uint32_t>(poseInfluence * 100.0f) ^ 0xdeadbeef);
				}

				morphName.append(pMesh->getName());
				morphName.append("_");
				morphName.append(KEP::getHexString(poseHash, 0));

				if (shaved)
				{
					hasInfluence = true;
					morphName.append("SLAVE");
				}

				if (!hasInfluence)
					return entity;

				existMesh = meshMgr.resourceExists(morphName);
			}
			else // 容姿がランダムの場合
			{
				auto& raceStringID = appearanceData->getFromList("race", 0);

				auto race = ou->gamedata.getData(raceStringID, RACE);
				morphName.append(race->stringID);
				morphName.append("_");
				morphName.append(Ogre::StringConverter::toString(std::floorf(ugliness * 10.0f)));
				morphName.append("_");
				morphName.append(appearanceData->bdata["sex female"] ? "Female" : "Male");
				morphName.append(Ogre::StringConverter::toString(iter->second));

				uint32_t seed = 0x811c9dc5;
				for (size_t i = 0; i < morphName.size(); ++i)
				{
					seed = seed * 0x1000193 ^ morphName[i];
				}

				if (shaved)
					morphName.append("SLAVE");

				existMesh = meshMgr.resourceExists(morphName);

				srand(seed); // RE_Kenshiの乱数修正を適用すると副作用で決定性が破壊されるバグが発生する
				self->randomPoses(appearanceData, ugliness);
			}
		}

		if (existMesh)
			entity = AppearanceManager::switchBodyMesh(entity, meshMgr.getByName(morphName));
		else
		{
			Ogre::LogManager::getSingleton().logMessage("Created Morph " + morphName);
			entity = AppearanceManager::createMorphedMesh(morphName, entity, appearanceData, shaved);
		}

		return entity;
	}

	void (*AppearanceHuman_setAttachmentsVisible_orig)(AppearanceHuman*, bool);
	void AppearanceHuman_setAttachmentsVisible_hook(AppearanceHuman* self, bool visible)
	{
		if (!KEP::settings._fixHairDisplay)
		{
			AppearanceHuman_setAttachmentsVisible_orig(self, visible);
			return;
		}

		if (self->body == nullptr)
			return;

		bool hideHair = false;
		bool hideBeard = false;
		for (auto iter = self->attachedObjects.begin(); iter != self->attachedObjects.end(); ++iter)
		{
			auto attach = iter->second;
			if (attach->data->idata["slot"] == 3 && attach->data->bdata["hide hair"])
				hideHair = true;

			if (attach->data->bdata["hide beard"])
				hideBeard = true;
		}

		for (auto iter = self->attachedObjects.begin(); iter != self->attachedObjects.end(); ++iter)
		{
			auto attach = iter->second;
			if ((attach->attachType == 0 || attach->attachType == 1) && attach->handle.type != LIMB_REPLACEMENT)
			{
				auto attachSlot = attach->data->idata["attach slot"];
				if (attachSlot == 0x2)
				{
					if (visible)
					{
						if (hideHair)
							attach->detach();
					}
					else
					{
						if (hideHair && attach->entity == nullptr)
						{
							self->setupHairMaterial(attach);
							self->createEntity(attach);
						}
					}
				}
				else if (attachSlot == 0xd)
				{
					if (visible)
					{
						if (hideBeard)
							attach->detach();
					}
					else
					{
						if (hideBeard && attach->entity == nullptr)
						{
							self->setupHairMaterial(attach);
							self->createEntity(attach);
						}
					}
				}
				else if (attach->entity != nullptr)
					attach->entity->setVisible(visible);
			}
		}

		self->hideAttachments = !visible;
		self->hiddenPartsEnabled = visible;
		self->updateCharaterTexture();
	}

	AppearanceManager* (*AppearanceManager__CONSTRUCTOR_orig)(AppearanceManager*);
	AppearanceManager* AppearanceManager__CONSTRUCTOR_hook(AppearanceManager* self)
	{
		AppearanceManager__CONSTRUCTOR_orig(self);
		std::sort(self->characterIdleAnimations.begin(), self->characterIdleAnimations.end(), AnimationDataLess());
		return self;
	}

	void (*AppearanceManager_updateModifiers_orig)(AppearanceManager*, GameData*, GameData*, const AppearanceManager::Gender&);
	void AppearanceManager_updateModifiers_hook(AppearanceManager* self, GameData* appearanceData, GameData* raceData, const AppearanceManager::Gender& gender)
	{
		auto headData = ou->gamedata.getData(appearanceData->sdata["head"], HEAD);
		AppearanceManager_updateModifiers_orig(self, appearanceData, raceData, gender);
		if (KEP::settings._fixHairDisplay && headData != nullptr && self->getHeadIsValid(raceData, appearanceData, headData))
			appearanceData->sdata["head"] = headData->stringID;
	}
}

void KEP::AppearanceFix::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&AppearanceManager::getMorphedBodyMesh), &AppearanceManager_getMorphedBodyMesh_hook, &AppearanceManager_getMorphedBodyMesh_orig))
		ErrorLog("[AppearanceManager::getMorphedBodyMes] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&AppearanceHuman::_NV_setAttachmentsVisible), &AppearanceHuman_setAttachmentsVisible_hook, &AppearanceHuman_setAttachmentsVisible_orig))
		ErrorLog("[AppearanceHuman::setAttachmentsVisible] could not install hook!");

	if (settings._sortedIdleStances)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&AppearanceManager::_CONSTRUCTOR), &AppearanceManager__CONSTRUCTOR_hook, &AppearanceManager__CONSTRUCTOR_orig))
			ErrorLog("[AppearanceManager::AppearanceManager] could not install hook!");
	}

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&AppearanceManager::updateModifiers), &AppearanceManager_updateModifiers_hook, &AppearanceManager_updateModifiers_orig))
		ErrorLog("[AppearanceManager::updateModifiers] could not install hook!");
}
