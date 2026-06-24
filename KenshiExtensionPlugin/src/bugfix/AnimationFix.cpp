#include <boost/filesystem.hpp>

#include <ogre/OgreLogManager.h>
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
#include <kenshi/OptionsHolder.h>
#include <kenshi/GameWorld.h>
#include <kenshi/KingOfRenderThread.h>
#include <kenshi/util/UtilityT.h>
#include <kenshi/Logger.h>
#include <kenshi/Renderer.h>
#include <kenshi/GameData.h>
#include <kenshi/Faction.h>
#include <kenshi/RaceData.h>
#include <kenshi/Character.h>
#include <kenshi/AppearanceManager.h>
#include <kenshi/Appearance.h>
#include <kenshi/RenderToTextureotron.h>
#include <kenshi/Animation/AnimationClass.h>

#include <kep/functions.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <bugfix/AnimationFix.h>

namespace fs = boost::filesystem;

namespace
{
	int compareModIndex(const void* a, const void* b)
	{
		if ((*(GameData**)a)->createdIndex < (*(GameData**)b)->createdIndex)
			return 1;
		return ((*(GameData**)a)->createdIndex <= (*(GameData**)b)->createdIndex) - 1;
	}

	struct GreaterAnimationFile
	{
		bool operator()(const GameData* a, const GameData* b) const
		{
			if (a->createdIndex != b->createdIndex)
				return b->createdIndex < a->createdIndex;
			return b->stringID < a->stringID;
		}
	};

	class DetourSkeleton : public Ogre::Skeleton
	{
	public:
		void _refreshAnimationState_hook(Ogre::AnimationStateSet* animSet);
	};

	void (Ogre::Skeleton::* Ogre_Skeleton__refreshAnimationState_orig)(Ogre::AnimationStateSet*);
	void DetourSkeleton::_refreshAnimationState_hook(Ogre::AnimationStateSet* animSet)
	{
		if (!KEP::settings._fixAnimationOverride)
		{
			(this->*Ogre_Skeleton__refreshAnimationState_orig)(animSet);
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

	void (*AppearanceBase_buildBody_orig)(AppearanceBase*, Ogre::SharedPtr<Ogre::Resource>, void*);
	void AppearanceBase_buildBody_hook(AppearanceBase* self, Ogre::SharedPtr<Ogre::Resource> _a1, void* bodyData)
	{
		if (!KEP::settings._fixAnimationOverride)
		{
			AppearanceBase_buildBody_orig(self, _a1, bodyData);
			return;
		}

		self->isCreatingBody = false;
		auto& pMesh = self->bodyLoader->getMesh();
		KEP::functions->loadPartmap(pMesh, self->raceData, self->female, self->me->isAnimal() == nullptr);

		if (!self->appearanceData->bdata["in editor"])
			self->body = AppearanceManager::getInstance()->getMorphedBodyMesh(self->appearanceData, self->bodyLoader, self->me->isPlayerCharacter(), self->me->getFaction()->data->fdata["faces weirdness"], self->shaved);
		else
			self->body = AppearanceManager::getInstance()->getBodyMeshForEditor(self->bodyLoader);

		self->bodyLoader = nullptr;
		self->body->getParentSceneNode()->setPosition(self->me->getPosition());
		self->body->getParentSceneNode()->setOrientation(self->me->getOrientation());
		self->body->setCastShadows(true);
		self->body->setRenderingDistance(options->NPCRange);
		auto aabb = self->body->getLocalAabb();

		float radius = aabb.mHalfSize.z;
		if (aabb.mHalfSize.z <= aabb.mHalfSize.y)
			radius = aabb.mHalfSize.y;
		if (aabb.mHalfSize.y <= aabb.mHalfSize.x)
			radius = aabb.mHalfSize.x;
		self->bodyRadius = radius;
		self->body->setLocalAabb(Ogre::Aabb(aabb.mCenter, Ogre::Vector3(radius)));
		if (RaceData::getRaceData(self->raceData)->gigantic)
			self->body->setRenderingDistance(options->NPCRange * 7.0f);

		auto skeleton = self->body->getSkeleton();
		if (skeleton == nullptr)
		{
			Logger::logMessage("[AppearanceBase::buildBody] Loaded body entity \'" + self->body->getMesh()->getName() + "\' for race \'" + self->raceData->name + "\' has no skeleton.", Logger::Error);
			self->isCreatingBody = true;
			self->body = nullptr;
			return;
		}

		auto numBones = skeleton->getNumBones();
		for (uint16_t i = 0; i < numBones; ++i)
		{
			skeleton->getBone(i)->setInheritScale(false);
		}

		auto headBone = skeleton->getBone("Bip01 Head");
		auto jawBone = skeleton->getBone("Bip01 Jaw");
		if (headBone != nullptr)
		{
			auto numChildren = headBone->numChildren();
			for (uint16_t i = 0; i < numChildren; ++i)
			{
				auto bone = skeleton->getBone(i);
				if (bone == jawBone)
					bone->setInheritScale(true);
			}
		}

		lektor<GameData*> animList;
		self->me->getGameData()->getAllFromListAsDatas("animation files", animList, &ou->gamedata, ANIMATION_FILE);
		if (animList.size() != 0)
			std::sort(animList.begin(), animList.end(), GreaterAnimationFile());

		lektor<GameData*> raceAnimList;
		self->raceData->getAllFromListAsDatas("animation files", raceAnimList, &ou->gamedata, ANIMATION_FILE);
		if (raceAnimList.size() != 0)
			std::sort(raceAnimList.begin(), raceAnimList.end(), GreaterAnimationFile());

		for (uint32_t i = 0; i < raceAnimList.size(); ++i)
		{
			animList.push_back(raceAnimList[i]);
		}

		auto& resourceMgr = Ogre::ResourceGroupManager::getSingleton();
		auto& logMgr = Ogre::LogManager::getSingleton();
		std::string keyName = self->female ? "female animation" : "male animation";

		auto numAnimFiles = animList.size();
		if (numAnimFiles != 0)
		{
			for (uint32_t i = 0; i < numAnimFiles; ++i)
			{
				fs::path resourcePath = animList[i]->filesdata[keyName];
				if (!resourcePath.empty())
				{
					std::string filename = resourcePath.filename().string();
					if (resourceMgr.resourceExistsInAnyGroup(filename))
					{
						skeleton->addLinkedSkeletonAnimationSource(filename, -1.0f);
					}
					else
					{
						logMgr.logMessage("Error: Skeleton file " + filename + " not found");
					}
				}
			}
			skeleton->_refreshAnimationState(self->body->getAllAnimationStates());
		}

		self->animation->setBodyEntity(self->body);
		if (self->me != nullptr && self->me->isPlayerCharacter())
			ou->addPortraitUpdate(self->me->getHandle());

		self->animation->notifyRagdollNeedsAnUpdate();
		self->updateAppearance();
		self->updateCharaterTexture();
		self->buildAttachments();
		self->updateBloodyness();

		self->body->getParentSceneNode()->setVisible(self->me->getVisible());
	}

	void (*RenderToTextureotron_renderCharacter_orig)(RenderToTextureotron*, Character*, Ogre::SharedPtr<Ogre::Texture>, const iVector2&, const Ogre::TRect<float>&);
	void RenderToTextureotron_renderCharacter_hook(RenderToTextureotron* self, Character* character, Ogre::SharedPtr<Ogre::Texture> texture, const iVector2& size, const Ogre::TRect<float>& rect)
	{
		if (!KEP::settings._fixPortrait)
		{
			RenderToTextureotron_renderCharacter_orig(self, character, texture, size, rect);
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
			self->sceneManager = au->render->root->createSceneManager(1, 1, Ogre::INSTANCING_CULLING_SINGLETHREAD);

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
		if (KEP::settings._fixAnimationOverride)
		{
			character->getGameData()->getAllFromListAsDatas(referenceName, animationFiles, &ou->gamedata, ANIMATION_FILE);
			if (animationFiles.size() != 0)
				std::sort(animationFiles.begin(), animationFiles.end(), GreaterAnimationFile());

			lektor<GameData*> raceAnimationFiles;
			if (race != nullptr && race->data != nullptr)
				race->data->getAllFromListAsDatas(referenceName, raceAnimationFiles, &ou->gamedata, ANIMATION_FILE);
			if (raceAnimationFiles.size() != 0)
				std::sort(raceAnimationFiles.begin(), raceAnimationFiles.end(), GreaterAnimationFile());

			for (uint32_t i = 0; i < raceAnimationFiles.size(); ++i)
			{
				animationFiles.push_back(raceAnimationFiles[i]);
			}
		}
		else
		{
			if (race != nullptr && race->data != nullptr)
				race->data->getAllFromListAsDatas(referenceName, animationFiles, &ou->gamedata, ANIMATION_FILE);
			character->getGameData()->getAllFromListAsDatas(referenceName, animationFiles, &ou->gamedata, ANIMATION_FILE);
			qsort(animationFiles.begin(), animationFiles.size(), 8, compareModIndex);
		}

		auto numAnimFiles = animationFiles.size();
		if (numAnimFiles != 0)
		{
			for (uint32_t i = 0; i < numAnimFiles; ++i)
			{
				fs::path resourcePath = animationFiles[i]->filesdata[keyName];
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
		}

		if (character->isHuman() != nullptr)
		{
			portraitCharacterSkeleton->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

			auto bodyGameData = character->animation->appearance->appearanceData;

			auto animListsMgr = AnimsListsManager::getSingleton();
			if (animListsMgr->characterAnimations.allAnims.size() == 0)
				animListsMgr->setupCharacterList();

			auto idleAnimData = animListsMgr->characterAnimations.allAnims[bodyGameData->sdata["idle stance"]];
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

		Ogre::Radian radY((static_cast<float>(character->portraitSerial % 500) / 1000.0f - 0.25f) * Ogre::Math::PI);
		rotateY.FromAngleAxis(radY, Ogre::Vector3::UNIT_Y);
		Ogre::Radian radX((static_cast<float>(character->portraitSerial % 252) / 1000.0f - 0.083f) * Ogre::Math::PI);
		rotateX.FromAngleAxis(radX, Ogre::Vector3::UNIT_X);

		portraitDistancePosition = rotateY * rotateX * portraitDistancePosition;

		self->setupCamera(size, 25.0f, 400.0f);

		self->camera->setFixedYawAxis(true);
		self->camera->setPosition(portraitCenterPosition + portraitDistancePosition);
		self->camera->lookAt(portraitCenterPosition);

		Ogre::IdString idDeferredLighting;
		MurmurHash3_x86_32("RTT_Deferred_Lighting", 21, 0x3a8efa67, &idDeferredLighting);
		auto compositor = au->render->root->getCompositorManager2();
		auto v = compositor->getNodeDefinitionNonConst(idDeferredLighting)->getTargetPass(0)->getCompositorPasses()[0];
		v->mVpTop = rect.top;
		v->mVpLeft = rect.left;
		v->mVpWidth = rect.right - rect.left;
		v->mVpHeight = rect.bottom - rect.top;
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

	void (*AnimationClass_processIdleAnims_orig)(AnimationClass*, lektor<AnimationData*>*, AnimationRequirement&, float);
	void AnimationClass_processIdleAnims_hook(AnimationClass* self, lektor<AnimationData*>* list, AnimationRequirement& requirements, float frameTIME)
	{
		if (!KEP::settings._fixIdleAnimation)
		{
			AnimationClass_processIdleAnims_orig(self, list, requirements, frameTIME);
			return;
		}

		if (self->animationRequirements.smoothedMoveSpeed != 0.0f)
			return;

		auto idleState = self->animationRequirements.idleState;
		self->animationRequirements.idleTimer -= frameTIME;

		if (idleState != nullptr && !idleState->looped)
		{
			auto singleAnim = self->getAnimationPlaying(idleState);
			if (singleAnim == nullptr || 0.98f < singleAnim->currentFrameTime01)
			{
				self->animationRequirements.idleTimer = -1.0f;
				if (singleAnim != nullptr)
					singleAnim->currentFrameTime = 0.0f;
			}
		}

		if (idleState != nullptr)
		{
			if (self->scoreAnim(idleState, requirements, false) <= 0.0f)
			{
				if (self->animationRequirements.currentAnimCategory != ANIM_NORMAL
					|| !self->animationRequirements.carried && !self->animationRequirements.carryingLeft && !self->animationRequirements.carryingRight && (idleState->layername == UPPER || idleState->layername == OVERLAY)
					|| self->scoreAnim(idleState, requirements, true) <= 0.0f)
					self->animationRequirements.idleState = nullptr;
			}
		}

		idleState = self->animationRequirements.idleState;
		if (idleState != nullptr && idleState->category != self->animationRequirements.currentAnimCategory || self->animationRequirements.idleTimer < 0.0f || idleState == nullptr)
		{
			idleState = self->getIdleAnimation();
			self->animationRequirements.idleState = idleState;
			self->animationRequirements.idleTimer = UtilityT::random() * idleState->idleTimeMax + idleState->idleTimeMin;
			if (idleState == nullptr)
			{
				self->isIdle = true;
				return;
			}
		}

		self->runAnimation(idleState, idleState->playSpeed, idleState->layername, 1.0f);
	}
}

void KEP::AnimationFix::init()
{
	void (*Ogre_Skeleton__refreshAnimationState)(Ogre::Skeleton*, Ogre::AnimationStateSet*) = nullptr;
	auto hOgreMain = GetModuleHandleA("OgreMain_x64.dll");
	*(uintptr_t*)&Ogre_Skeleton__refreshAnimationState = reinterpret_cast<uintptr_t>(GetProcAddress(hOgreMain, "?_refreshAnimationState@Skeleton@Ogre@@UEAAXPEAVAnimationStateSet@2@@Z"));
	auto pfuncTarget = &DetourSkeleton::_refreshAnimationState_hook;
	auto pfuncOrig = &Ogre_Skeleton__refreshAnimationState_orig;
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(Ogre_Skeleton__refreshAnimationState, *(void**)&pfuncTarget, *(void***)&pfuncOrig))
		ErrorLog("[Ogre::Skeleton::_refreshAnimationState] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&AppearanceBase::buildBody), AppearanceBase_buildBody_hook, &AppearanceBase_buildBody_orig))
		ErrorLog("[AppearanceBase::buildBody] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&RenderToTextureotron::renderCharacter), &RenderToTextureotron_renderCharacter_hook, &RenderToTextureotron_renderCharacter_orig))
		ErrorLog("[RenderToTextureotron::renderCharacter] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&AnimationClass::processIdleAnims), AnimationClass_processIdleAnims_hook, &AnimationClass_processIdleAnims_orig))
		ErrorLog("[AnimationClass::processIdleAnims] Could not add hook!");
}
