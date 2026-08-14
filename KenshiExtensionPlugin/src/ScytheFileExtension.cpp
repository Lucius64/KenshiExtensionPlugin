/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
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
	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(externalFunctions->FUN_001A4FB0, &PhysicsEntity_loadingPart_hook, &PhysicsEntity_loadingPart_orig))
		ErrorLog("[PhysicsEntity::loadingPart] could not install hook!");
}
