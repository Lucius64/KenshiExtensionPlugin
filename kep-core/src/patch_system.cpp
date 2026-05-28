#include "pch.h"

#include <boost/filesystem.hpp>
#include <boost/math/constants/constants.hpp>

#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreResourceGroupManager.h>
#include <ogre/OgreStringConverter.h>
#include <ogre/OgreConfigFile.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/GlobalConstants.h>

#include <kep/translation.h>
#include <kep/settings.h>
#include <kep/patch_system.h>

namespace fs = boost::filesystem;

namespace
{
	void loadPatchMod(GameDataContainer* self, const Ogre::String& filePath, int modIndex)
	{
		KEP::PatchSystem::ModReader reader(filePath);

		if (reader.isErr())
		{
			ErrorLog("Mod file open error " + filePath);
			return;
		}

		auto fileVersion = reader.readInt32();

		if (fileVersion < 15 || 17 < fileVersion)
		{
			ErrorLog("Invalid data file " + filePath);
			return;
		}

		auto& resourceMgr = Ogre::ResourceGroupManager::getSingleton();
		std::vector<boost::tuple<GameData*, std::string, std::string, int, int, int>> objectReferences;

		reader.readHeader(fileVersion);

		auto lastID = reader.readInt32();
		auto recordCount = reader.readInt32();
		for (int32_t numRecord = 0; numRecord < recordCount; ++numRecord)
		{
			reader.ignore(4);
			itemType dataType = static_cast<itemType>(reader.readUInt32());
			auto recordNumber = reader.readInt32();
			auto name = reader.readString();
			auto stringID = reader.readString();

			auto flag = reader.readInt32() & 0x7fffffff;
			bool modified = (flag & 1) != 0;
			bool renamed = (~flag & 2) == 0;

			auto gameData = self->getData(stringID);
			if (gameData == nullptr && !modified)
			{
				gameData = self->createNewData(dataType, stringID, name);
				gameData->createdIndex = modIndex;
			}

			if (gameData != nullptr)
			{
				if (renamed)
					self->renameData(gameData, name);

				auto itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					auto key = reader.readString();
					gameData->bdata[key] = reader.readBoolean();
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					auto key = reader.readString();
					gameData->fdata[key] = reader.readFloat();
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					auto key = reader.readString();
					gameData->idata[key] = reader.readInt32();
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					auto key = reader.readString();
					gameData->vecdata[key] = reader.readVector3();
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					auto key = reader.readString();
					gameData->quatdata[key] = reader.readQuaternion();
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					auto key = reader.readString();
					gameData->sdata[key] = reader.readString();
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					auto key = reader.readString();
					auto val = reader.readString();
					if (!val.empty())
					{
						fs::path resourcePath(val);
						if (resourceMgr.resourceExistsInAnyGroup(resourcePath.filename().string()))
						{
							gameData->filesdata[key] = val;
						}
						else if (resourcePath.is_relative())
						{
							auto iter = resourcePath.begin();
							if (iter != resourcePath.end() && iter->string() == "data" && fs::exists(resourcePath))
								gameData->filesdata[key] = val;
						}
					}
					else
					{
						gameData->filesdata[key] = val;
					}
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					auto key = reader.readString();
					auto referenceCount = reader.readInt32();
					for (int32_t numReference = 0; numReference < referenceCount; ++numReference)
					{
						auto referenceSID = reader.readString();
						TripleInt referenceValues;
						referenceValues.value[0] = reader.readInt32();
						referenceValues.value[1] = reader.readInt32();
						referenceValues.value[2] = reader.readInt32();

						objectReferences.push_back(boost::make_tuple(gameData, key, referenceSID, referenceValues.value[0], referenceValues.value[1], referenceValues.value[2]));
					}
				}

				auto instanceCount = reader.readInt32();
				for (int32_t numInstance = 0; numInstance < instanceCount; ++numInstance)
				{
					auto instanceID = reader.readString();
					auto referenceStringID = reader.readString();
					auto pos = reader.readVector3();
					auto rot = reader.readQuaternion();

					lektor<Ogre::String> states;
					auto stateCount = reader.readInt32();
					for (int32_t numState = 0; numState < stateCount; ++numState)
					{
						states.push_back(reader.readString());
					}

					if (!referenceStringID.empty())
					{
						gameData->addSavedInstancedObject(instanceID, referenceStringID, modIndex, pos, rot, states);
					}
					else
					{
						gameData->deleteInstancedObject(instanceID, modIndex);
					}
				}

				auto iter = gameData->bdata.find("REMOVED");
				if (iter != gameData->bdata.end() && iter->second)
					self->removeData(gameData);

			}
			else //dispose
			{
				auto itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					reader.readString();
					reader.readBoolean();
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					reader.readString();
					reader.readFloat();
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					reader.readString();
					reader.readInt32();
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					reader.readString();
					reader.readVector3();
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					reader.readString();
					reader.readQuaternion();
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					reader.readString();
					reader.readString();
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					reader.readString();
					reader.readString();
				}

				itemCount = reader.readInt32();
				for (int32_t numItem = 0; numItem < itemCount; ++numItem)
				{
					reader.readString();
					auto referenceCount = reader.readInt32();
					for (int32_t numReference = 0; numReference < referenceCount; ++numReference)
					{
						reader.readString();
						reader.readInt32();
						reader.readInt32();
						reader.readInt32();
					}
				}

				auto instanceCount = reader.readInt32();
				for (int32_t numInstance = 0; numInstance < instanceCount; ++numInstance)
				{
					reader.readString();
					reader.readString();
					reader.readVector3();
					reader.readQuaternion();
					auto stateCount = reader.readInt32();
					for (int32_t numState = 0; numState < stateCount; ++numState)
					{
						reader.readString();
					}
				}
			}
		}

		auto endIt = objectReferences.end();
		for (auto iter = objectReferences.begin(); iter != endIt; ++iter)
		{
			if (iter->get<3>() == 0x7fffffff && iter->get<4>() == 0x7fffffff && iter->get<5>() == 0x7fffffff)
			{
				iter->get<0>()->removeFromList(iter->get<1>(), iter->get<2>());
			}
			else
			{
				auto referenceData = self->getData(iter->get<2>());
				if (referenceData != nullptr)
					iter->get<0>()->addToList(iter->get<1>(), iter->get<2>(), iter->get<3>(), iter->get<4>(), iter->get<5>());
			}
		}
	}

	void (*GameDataManager_postProcessingTheDatas_orig)(GameDataManager*);
	void GameDataManager_postProcessingTheDatas_hook(GameDataManager* self)
	{
		auto& language = KEP::TranslationUtility::getLanguage();

		int32_t modIndex = ou->activeMods.size();
		for (auto iter = ou->activeMods.begin(); iter < ou->activeMods.end(); ++iter)
		{
			if ((*iter)->isBaseMod)
				continue;

			fs::path modPath((*iter)->path);
			modPath /= "patch";
			if (fs::exists(modPath))
			{
				std::set<std::string> files;
				for (fs::directory_iterator dirIter(modPath); dirIter != fs::directory_iterator(); ++dirIter)
				{
					if (!fs::is_directory(dirIter->path()) && dirIter->path().extension() == ".mod")
						files.emplace(dirIter->path().string());
				}
				for (auto fileIter = files.begin(); fileIter != files.end(); ++fileIter)
				{
					DebugLog("Loading " + *fileIter);
					loadPatchMod(self, *fileIter, modIndex);
				}

				modPath /= language;
				if (fs::exists(modPath))
				{
					std::set<std::string> localeFiles;
					for (fs::directory_iterator dirIter(modPath); dirIter != fs::directory_iterator(); ++dirIter)
					{
						if (dirIter->path().extension() == ".mod")
							localeFiles.emplace(dirIter->path().string());
					}
					for (auto fileIter = localeFiles.begin(); fileIter != localeFiles.end(); ++fileIter)
					{
						DebugLog("Loading " + *fileIter);
						loadPatchMod(self, *fileIter, modIndex);
					}
				}
			}
		}

		GameDataManager_postProcessingTheDatas_orig(self);

		auto constantsData = self->getDataByName("GLOBAL CONSTANTS", CONSTANTS);
		auto& weightStrDiff = constantsData->idata["weight strength diff 1x"];
		auto& weightStrDiffMax = constantsData->fdata["weight strength diff max"];

		if (weightStrDiff == 0 || !boost::math::isfinite(weightStrDiff))
		{
			weightStrDiff = 20;
			ErrorLog("\"weight strength diff 1x\" is an invalid value and has been converted to the default value.");
		}
		if (weightStrDiffMax == 0.0f || !boost::math::isfinite(weightStrDiffMax))
		{
			weightStrDiffMax = 1.0f;
			ErrorLog("\"weight strength diff max\" is an invalid value and has been converted to the default value.");
		}
	}
}

void KEP::PatchSystem::init()
{
	if (KEP::coreOptions.getEnablePatchSystem())
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&GameDataManager::postProcessingTheDatas), &GameDataManager_postProcessingTheDatas_hook, &GameDataManager_postProcessingTheDatas_orig))
			ErrorLog("[GameDataManager::postProcessingTheDatas] could not install hook!");
	}
}
