#include <boost/filesystem.hpp>

#include <ogre/OgreLogManager.h>
#include <ogre/OgreMeshManager.h>
#include <ogre/OgrePose.h>
#include <ogre/OgreStringConverter.h>
#include <ogre/OgreEntity.h>
#include <ogre/OgreRoot.h>
#include <ogre/OgreCamera.h>
#include <ogre/Compositor/OgreCompositorManager2.h>
#include <ogre/Compositor/OgreCompositorNodeDef.h>
#include <ogre/Compositor/OgreCompositorWorkspace.h>
#include <ogre/OgreIteratorWrapper.h>
#include <ogre/OgreOldSkeletonInstance.h>
#include <ogre/OgreOldBone.h>
#include <ogre/OgreHardwarePixelBuffer.h>
#include <ogre/OgreGpuProgramManager.h>
#include <ogre/OgreRenderTexture.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/Character.h>
#include <kenshi/RaceData.h>
#include <kenshi/AppearanceManager.h>
#include <kenshi/Appearance.h>
#include <kenshi/gui/PortraitManager.h>

#include <extern/AppearanceBase.h>
#include <extern/AnimationClass.h>
#include <extern/PortraitManager.h>
#include <extern/Renderer.h>

#include <UtilityFunction.h>
#include <ExternalFunctions.h>
#include <Settings.h>
#include <bugfix/AppearanceFix.h>


namespace fs = boost::filesystem;

namespace
{
	int compareModIndex(const void* a, const void* b)
	{
		if ((*(GameData**)a)->createdIndex < (*(GameData**)b)->createdIndex)
			return 1;
		return ((*(GameData**)a)->createdIndex <= (*(GameData**)b)->createdIndex) - 1;
	}

	struct AnimationDataLess
	{
		bool operator()(const AnimationData* a, const AnimationData* b) const
		{
			if (a->data->createdIndex != b->data->createdIndex)
				return a->data->createdIndex < b->data->createdIndex;

			std::string s1 = a->name;
			std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
			std::string s2 = b->name;
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

	void (*RenderToTexture_FUN_0084A9C0_orig)(RenderToTexture*, Character*, Ogre::SharedPtr<Ogre::Texture>, const PortraitManager&, const Ogre::Vector4&);
	void RenderToTexture_FUN_0084A9C0_hook(RenderToTexture* self, Character* character, Ogre::SharedPtr<Ogre::Texture> texture, const PortraitManager& portraitMgr, const Ogre::Vector4& texcoord)
	{
		if (!KEP::settings._fixPortrait)
		{
			RenderToTexture_FUN_0084A9C0_orig(self, character, texture, portraitMgr, texcoord);
			return;
		}

		Ogre::Entity* characterEntity = character->animation->appearance->body;

		Ogre::SceneNode* parentSceneNode;
		Ogre::OldSkeletonInstance* skeleton;

		if (characterEntity == nullptr)
		{
			parentSceneNode = nullptr;
			skeleton = nullptr;
		}
		else
		{
			parentSceneNode = characterEntity->getParentSceneNode();
			skeleton = characterEntity->getSkeleton();
		}

		if (self->sceneManager == nullptr)
			self->sceneManager = KEP::externalGlobals->_CLASS_021322B0->renderer->root->createSceneManager(1, 1, Ogre::INSTANCING_CULLING_SINGLETHREAD);

		Ogre::SceneNode* roottSceneNode = self->sceneManager->getRootSceneNode();
		Ogre::SceneNode* portraitSceneNode = roottSceneNode->createChildSceneNode();
		portraitSceneNode->setPosition(Ogre::Vector3::ZERO);

		Ogre::Entity* portraitCharacterEntity = nullptr;

		size_t numObj = parentSceneNode->numAttachedObjects();
		for (size_t i = 0; i < numObj; ++i)
		{
			auto obj = reinterpret_cast<Ogre::Entity*>(parentSceneNode->getAttachedObject(i));
			auto& meshPtr = obj->getMesh();
			auto ent = self->sceneManager->createEntity(meshPtr);
			auto& materialPtr = obj->getSubEntity(0)->getMaterial();
			ent->setMaterial(materialPtr);

			if (portraitCharacterEntity == nullptr)
			{
				if (obj->hasSkeleton())
					portraitCharacterEntity = ent;
			}
			else
			{
				ent->shareSkeletonInstanceWith(portraitCharacterEntity);
				ent->setSkipAnimationStateUpdate(true);
				ent->setLocalAabb(portraitCharacterEntity->getLocalAabb());
			}

			portraitSceneNode->attachObject(ent);
		}

		Ogre::OldSkeletonInstance* portraitCharacterSkeleton = portraitCharacterEntity->getSkeleton();

		auto boneIteratorConst = skeleton->getBoneIteratorConst();
		if (boneIteratorConst.current() != boneIteratorConst.end())
		{
			do
			{
				auto it = boneIteratorConst.peekNext();
				boneIteratorConst.moveNext();
				auto bone = portraitCharacterSkeleton->getBone(it->getHandle());
				bone->setBoneSize(it->getBoneSize());
				bone->setBonePositionalSize(it->getBonePositionalSize());
			} while (boneIteratorConst.current() != boneIteratorConst.end());
		}
		portraitCharacterSkeleton->setMovementScale(1.0f);

		auto& resourceMgr = Ogre::ResourceGroupManager::getSingleton();
		auto& logMgr = Ogre::LogManager::getSingleton();
		auto race = character->getRace();
		std::string keyName = character->animation->appearance->female ? "female animation" : "male animation";
		std::string referenceName = "animation files";

		lektor<GameData*> animationFiles;
		if (race != nullptr && race->data != nullptr)
			race->data->getAllFromListAsDatas(referenceName, animationFiles, &ou->gamedata, ANIMATION_FILE);

		character->data->getAllFromListAsDatas(referenceName, animationFiles, &ou->gamedata, ANIMATION_FILE);

		qsort(animationFiles.begin(), animationFiles.size(), 8, compareModIndex);

		for (auto it = animationFiles.begin(); it != animationFiles.end(); ++it)
		{
			fs::path resourcePath = (*it)->filesdata[keyName];
			if (!resourcePath.empty())
			{
				std::string filename = resourcePath.filename().string();
				if (resourceMgr.resourceExistsInAnyGroup(filename))
				{
					portraitCharacterSkeleton->addLinkedSkeletonAnimationSource(filename, -1.0f);
				}
				else
				{
					std::string logMsg = "Error: Skeleton file ";
					logMgr.logMessage(logMsg + filename + " not found");
				}
			}
		}
		portraitCharacterSkeleton->_refreshAnimationState(portraitCharacterEntity->getAllAnimationStates());

		if (character->isHuman() != nullptr)
		{
			portraitCharacterSkeleton->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

			auto bodyGameData = character->animation->appearance->appearanceData;

			auto animListsMgr = KEP::externalFunctions->FUN_000B70A0();
			if (animListsMgr->animList.enableAnims.size() == 0)
				KEP::externalFunctions->FUN_005BC680(animListsMgr);

			auto idleAnimData = animListsMgr->animList.enableAnims[bodyGameData->sdata["idle stance"]];
			if (idleAnimData != nullptr)
				KEP::externalFunctions->FUN_00844C90(portraitCharacterEntity, idleAnimData->animName, 0.0f);

			KEP::externalFunctions->FUN_00844C90(portraitCharacterEntity, "postures", bodyGameData->fdata["Posture"] * 0.01f);
			KEP::externalFunctions->FUN_00844C90(portraitCharacterEntity, "neck set", bodyGameData->fdata["Neck position"] * 0.01f);
			KEP::externalFunctions->FUN_00844C90(portraitCharacterEntity, "shoulder set", bodyGameData->fdata["Shoulder set"] * 0.01f);

			portraitSceneNode->_getFullTransformUpdated();
			portraitCharacterEntity->_updateAnimation();
		}

		Ogre::Vector3 portraitOffsetPosition(0.0f, 0.0f, 0.0f);
		Ogre::Vector3 portraitDistancePosition(0.0f, 0.0f, 0.0f);
		if (race != nullptr && race->data != nullptr)
		{
			portraitOffsetPosition.x = race->data->fdata["portrait offset x"];
			portraitOffsetPosition.y = race->data->fdata["portrait offset y"];
			portraitOffsetPosition.z = race->data->fdata["portrait offset z"];
			portraitOffsetPosition *= character->getAge();

			portraitDistancePosition.z = race->data->fdata["portrait distance"];
		}
		auto& headBonePos = portraitCharacterSkeleton->getBone("Bip01 Head")->_getDerivedPosition();
		auto scencePos = portraitSceneNode->getPosition();
		auto scenceScl = portraitSceneNode->getScale();

		Ogre::Vector3 portraitCenterPosition = scenceScl * headBonePos + scencePos + portraitOffsetPosition;

		Ogre::Quaternion rotateX;
		Ogre::Quaternion rotateY;

		Ogre::Radian radY((static_cast<float>(character->portraitSerial % 500) / 1000.0f - 0.25f)* Ogre::Math::PI);
		rotateY.FromAngleAxis(radY, Ogre::Vector3::UNIT_Y);
		Ogre::Radian radX((static_cast<float>(character->portraitSerial % 252) / 1000.0f - 0.083f)* Ogre::Math::PI);
		rotateX.FromAngleAxis(radX, Ogre::Vector3::UNIT_X);

		portraitDistancePosition = rotateY * rotateX * portraitDistancePosition;

		KEP::externalFunctions->FUN_00845060(self, portraitMgr.textureSize, 25.0f, 400.0f);

		self->camera->setFixedYawAxis(true);
		self->camera->setPosition(portraitCenterPosition + portraitDistancePosition);
		self->camera->lookAt(portraitCenterPosition);

		Ogre::IdString idDeferredLighting;
		MurmurHash3_x86_32("RTT_Deferred_Lighting", 21, 0x3a8efa67, &idDeferredLighting);
		auto compositor = KEP::externalGlobals->_CLASS_021322B0->renderer->root->getCompositorManager2();
		auto v = compositor->getNodeDefinitionNonConst(idDeferredLighting)->getTargetPass(0)->getCompositorPasses()[0];
		v->mVpTop = texcoord.y;
		v->mVpLeft = texcoord.x;
		v->mVpWidth = texcoord.z - texcoord.x;
		v->mVpHeight = texcoord.w - texcoord.y;
		auto renderTarget = texture->getBuffer()->getRenderTarget();

		Ogre::IdString idPortrait;
		MurmurHash3_x86_32("RTT_Portrait", 12, 0x3a8efa67, &idPortrait);
		auto workspace = compositor->addWorkspace(self->sceneManager, renderTarget, self->camera, idPortrait, true);

		Ogre::GpuProgramManager::getSingleton().getSharedParameters("SharedRTTViewMatrix")->setNamedConstant("inverseView", self->camera->getViewMatrix(true));

		self->sceneManager->updateSceneGraph();
		workspace->_update();
		renderTarget->swapBuffers();
		compositor->removeWorkspace(workspace);
		renderTarget->removeAllViewports();

		self->sceneManager->destroyAllEntities();
		roottSceneNode->removeAndDestroyAllChildren();
		v->mVpTop = 0.0f;
		v->mVpLeft = 0.0f;
		v->mVpWidth = 1.0f;
		v->mVpHeight = 1.0f;
	}

	class DetourSkeleton : public Ogre::Skeleton
	{
	public:
		void _refreshAnimationState_hook(Ogre::AnimationStateSet* animSet);
	};

	void (*Ogre_Skeleton__refreshAnimationState_orig)(Ogre::Skeleton*, Ogre::AnimationStateSet*);
	void DetourSkeleton::_refreshAnimationState_hook(Ogre::AnimationStateSet* animSet)
	{
		if (!KEP::settings._fixAnimationOverride)
		{
			Ogre_Skeleton__refreshAnimationState_orig(this, animSet);
			return;
		}

		Ogre::map<Ogre::uint32, Ogre::Animation*>::type::iterator iter;
		for (iter = this->mAnimationsList.begin(); iter != this->mAnimationsList.end(); ++iter)
		{
			Ogre::Animation* anim = iter->second;
			const std::string& animName = anim->getName();
			if (!animSet->hasAnimationState(iter->first))
			{
				animSet->createAnimationState(animName, iter->first, 0.0, anim->getLength());
			}
			else
			{
				Ogre::AnimationState* animState = animSet->getAnimationState(iter->first);
				animState->setLength(anim->getLength());
				animState->setTimePosition(std::min(anim->getLength(), animState->getTimePosition()));
			}
		}

		Ogre::Skeleton::LinkedSkeletonAnimSourceList::reverse_iterator rIter;
		for (rIter = this->mLinkedSkeletonAnimSourceList.rbegin();
			rIter != this->mLinkedSkeletonAnimSourceList.rend(); ++rIter)
		{
			if (!rIter->pSkeleton.isNull())
			{
				rIter->pSkeleton->_refreshAnimationState(animSet);
			}
		}
	}

	AppearanceManager* (*AppearanceManager__CONSTRUCTOR_orig)(AppearanceManager*);
	AppearanceManager* AppearanceManager__CONSTRUCTOR_hook(AppearanceManager* self)
	{
		AppearanceManager__CONSTRUCTOR_orig(self);
		std::sort(self->characterIdleAnimations.begin(), self->characterIdleAnimations.end(), AnimationDataLess());
		return self;
	}
}

void KEP::AppearanceFix::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&AppearanceManager::getMorphedBodyMesh), &AppearanceManager_getMorphedBodyMesh_hook, &AppearanceManager_getMorphedBodyMesh_orig))
		ErrorLog("[AppearanceManager::getMorphedBodyMes] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&AppearanceHuman::_NV_setAttachmentsVisible), &AppearanceHuman_setAttachmentsVisible_hook, &AppearanceHuman_setAttachmentsVisible_orig))
		ErrorLog("[AppearanceHuman::setAttachmentsVisible] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_0084A9C0, &RenderToTexture_FUN_0084A9C0_hook, &RenderToTexture_FUN_0084A9C0_orig))
		ErrorLog("[FUN_0084A9C0] could not install hook!");

	void (*Ogre_Skeleton__refreshAnimationState)(Ogre::Skeleton*, Ogre::AnimationStateSet*);
	auto hOgreMain = GetModuleHandleA("OgreMain_x64.dll");
	*(uintptr_t*)&Ogre_Skeleton__refreshAnimationState = reinterpret_cast<uintptr_t>(GetProcAddress(hOgreMain, "?_refreshAnimationState@Skeleton@Ogre@@UEAAXPEAVAnimationStateSet@2@@Z"));
	auto pfuncTarget = &DetourSkeleton::_refreshAnimationState_hook;
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(Ogre_Skeleton__refreshAnimationState, *(void**)&pfuncTarget, &Ogre_Skeleton__refreshAnimationState_orig))
		ErrorLog("[Ogre::Skeleton::_refreshAnimationState] could not install hook!");

	if (settings._sortedIdleStances)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&AppearanceManager::_CONSTRUCTOR), &AppearanceManager__CONSTRUCTOR_hook, &AppearanceManager__CONSTRUCTOR_orig))
			ErrorLog("[AppearanceManager::AppearanceManager] could not install hook!");
	}
}
