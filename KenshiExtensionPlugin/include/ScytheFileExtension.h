#pragma once

class PhysicsEntity;

namespace KEP
{
	namespace ScytheFileExtension
	{
		int PhysicsEntity_FUN_001A4FB0_hook(PhysicsEntity* self, const char* filePath);
		void init();
	}
}