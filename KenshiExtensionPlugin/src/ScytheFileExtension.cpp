#include <boost/filesystem.hpp>

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
		ErrorLog("KenshiExtensionPlugin: [Scythe file extension] could not install hook!");
}
