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
	int (*PhysicsEntity_loadingPart_orig)(scythe::data::PhysicsEntity*, const char*);
	int PhysicsEntity_loadingPart_hook(scythe::data::PhysicsEntity* self, const char* filePath)
	{
		std::string path(filePath);

		if (KEP::settings._scythePathExtension && path.size() != 0)
		{
			fs::path scythePath = path;
			std::string filename = scythePath.filename().string();
			UtilityT::getResourceFilePath(filename, path);
		}

		return PhysicsEntity_loadingPart_orig(self, path.c_str());
	}
}

void KEP::ScytheFileExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_001A4FB0, &PhysicsEntity_loadingPart_hook, &PhysicsEntity_loadingPart_orig))
		ErrorLog("[PhysicsEntity::loadingPart] could not install hook!");
}
