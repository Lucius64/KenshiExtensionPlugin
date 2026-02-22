#include <boost/filesystem.hpp>

#include <ogre/OgreResourceGroupManager.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>
#include <kenshi/util/UtilityT.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <ScytheFileExtension.h>

namespace fs = boost::filesystem;


namespace
{
	int (*PhysicsEntity_FUN_001A4FB0_orig)(PhysicsEntity*, const char*);
	void (*UtilityT_getResourceFilePath_orig)(const std::string&, std::string&);

	void UtilityT_getResourceFilePath_hook(const std::string& filename, std::string& filepath)
	{
		if (!KEP::settings._scythePathExtension)
		{
			UtilityT_getResourceFilePath_orig(filename, filepath);
			return;
		}

		auto resourceMgr = Ogre::ResourceGroupManager::getSingletonPtr();
		if (resourceMgr->resourceExistsInAnyGroup(filename))
		{
			auto fileInfoList = resourceMgr->findResourceFileInfo(resourceMgr->findGroupContainingResource(filename), filename);
			if (fileInfoList->size() != 0)
			{
				const auto& fileInfo = fileInfoList->back();
				filepath.assign(fileInfo.archive->getName());
				filepath.append("\\").append(fileInfo.filename);
			}
		}
	}
}

int KEP::ScytheFileExtension::PhysicsEntity_FUN_001A4FB0_hook(PhysicsEntity* self, const char* filePath)
{
	std::string path(filePath);

	if (settings._scythePathExtension && path.size() != 0)
	{
		fs::path scythePath = path;
		std::string filename = scythePath.filename().string();
		UtilityT::getResourceFilePath(filename, path);
	}

	return PhysicsEntity_FUN_001A4FB0_orig(self, path.c_str());
}

void KEP::ScytheFileExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_001A4FB0, &PhysicsEntity_FUN_001A4FB0_hook, &PhysicsEntity_FUN_001A4FB0_orig))
		ErrorLog("KenshiExtensionPlugin: [Scythe file extension pt1] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(UtilityT::getResourceFilePath), &UtilityT_getResourceFilePath_hook, &UtilityT_getResourceFilePath_orig))
		ErrorLog("KenshiExtensionPlugin: [Scythe file extension pt2] could not install hook!");
}
