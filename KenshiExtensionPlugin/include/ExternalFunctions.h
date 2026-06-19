#pragma once
#include <vadefs.h>
#include <cstdint>
#include <string>

#include <boost/scoped_ptr.hpp>

#include <ogre/OgrePrerequisites.h>

enum WorldStateEnum;
enum itemType;
class GameData;
class UniqueNPCManager;
class Faction;
class Character;
class CampaignInstance;
class AreaSector;
class Platoon;
class CampaignData;
class hand;
class TownBase;
class Town;
class UtilityT;
class TerrainDecalsManager;
class DistantTown;
class Research;
template<typename T>
class lektor;
class Blackboard;
class Task_FillMachine;
class StorageBuilding;
class Inventory;
template<typename T>
class FitnessSelector;
class GameDataReference;
class AnimalInventoryLayout;
class InventoryGUI;
class InventorySectionGUI;
class GameDataGroup;
class StringPair;
class GameplayOptions;

namespace scythe
{
	namespace data
	{
		class PhysicsEntity;
	}
}

namespace MyGUI
{
	class ItemBox;

	namespace types
	{
		template<typename T>
		struct TSize;
		template<typename T>
		struct TCoord;
	}
}

namespace KEP
{
	class ExternalGlobals
	{
	public:
		//placeholder name
		class BaseXpCombat
		{
		public:
			float _skillXp;
			float _attributeXp;
		};

		BaseXpCombat* _gBaseXpCombat;
		GameplayOptions* _optionsAdvanced;
		uint32_t* _playerMorphCounter;
		std::string* _KenshiFolder;
		UtilityT* _UtilityT;
		GameData* _TemporaryLimbInterface;
		MyGUI::types::TSize<int>* _InventorySlotSkinSize;
		MyGUI::types::TSize<float>* _ScreenSize;

		uintptr_t _dropItemReturnAddress;

		ExternalGlobals();
		void init(unsigned int platform, const std::string& version, uintptr_t baseAddr);

	};

	class ExternalFunctions
	{
	public:
		UniqueNPCManager& (*FUN_00354500)(void); // Get UniqueNPCManager singleton
		void (*FUN_00505D50)(UniqueNPCManager&, GameData*, const hand&); // Remove UniqueCharacterState
		bool (*FUN_00591720)(UniqueNPCManager&, GameData*); // Has not UniqueCharacterState
		WorldStateEnum (*FUN_005E7D60)(UniqueNPCManager&, GameData*); // Get character world state
		void (*FUN_009A7AA0)(UniqueNPCManager&); // Init UniqueCharacterState
		void (*FUN_009A93A0)(UniqueNPCManager&, GameData*, bool); // Load UniqueCharacterState
		bool (*FUN_009C4000)(CampaignData*); // Check world state
		bool (*FUN_009C3510)(Platoon*, CampaignInstance*, TownBase*); // Participate in events
		lektor<TownBase*>* (*FUN_009429B0)(UtilityT*, const Ogre::Vector3&, lektor<TownBase*>&, int); // List of nearest towns
		bool (*FUN_008F4200)(AreaSector*, const Ogre::Vector3&, bool, Faction*, float, float); // Calculate spawn position
		void (*FUN_00844C90)(Ogre::Entity* entity, const std::string& animName, float frame); // Set animation time position
		float(*FUN_008DBF80)(TerrainDecalsManager*, uint8_t, Ogre::Vector3&, Ogre::Vector3&, float, Ogre::ColourValue&, float); // 
		float(*FUN_008DC0D0)(TerrainDecalsManager*, Character*, Ogre::Vector3&, float, Ogre::ColourValue&);
		DistantTown* (*FUN_000D5FD0)(DistantTown*, Town*); // DistantTown(Town*)
		void (*FUN_00830B90)(Research*, GameData*); // Set BuildingUpgrades
		void (*FUN_002715E0)(Blackboard*, GameData*); // Set squad AI package
		int (*FUN_00340EB0)(Task_FillMachine*, StorageBuilding*, Inventory*);
		int (*FUN_00343720)(Task_FillMachine*, StorageBuilding*, Inventory*);
		int (*FUN_001A4FB0)(scythe::data::PhysicsEntity*, const char*); // Load scythe file
		void (*FUN_00954340)(FitnessSelector<uint32_t>&, GameData*); // crossbow level selector
		AnimalInventoryLayout* (*FUN_00155350)(AnimalInventoryLayout*); // AnimalInventoryLayout::AnimalInventoryLayout()
		void (*FUN_0014F530)(AnimalInventoryLayout*, InventoryGUI*, Ogre::map<std::string, InventorySectionGUI*>::type*, Inventory*); // Create GUI
		void (*FUN_000D8620)(DistantTown*, bool); // Update DistantTown
		std::string* (*FUN_000D6360)(DistantTown*, std::string*); // Get DistantMesh Filename
		void (*FUN_000D8310)(DistantTown*); // Create DistantMesh
		void (*FUN_004B0B60)(void*, MyGUI::ItemBox*, MyGUI::types::TCoord<int>&, bool); // ResearchItemList
		void (*FUN_002C9840)(void*, MyGUI::ItemBox*, MyGUI::types::TCoord<int>&, bool); // CraftItemList
		void (*FUN_0082E2E0)(Research*, lektor<GameDataGroup>&, itemType, itemType); // Get EnableItems
		void (*FUN_002CA350)(const Ogre::vector<GameDataReference>::type*, itemType, lektor<GameDataGroup>&); // Get EnableItems
		void (*FUN_0079EAB0)(Ogre::vector<StringPair>::type&, const std::string&, float, bool); // tooltip +-

		hand(UniqueNPCManager::* FUN_009AFCA0)(GameData*); // Get UniqueCharacterHandle (thiscall & RVO)

		ExternalFunctions();
		void init(unsigned int platform, const std::string& version, uintptr_t baseAddr);
	};

	extern boost::scoped_ptr<ExternalFunctions> externalFunctions;
	extern boost::scoped_ptr<ExternalGlobals> externalGlobals;
}