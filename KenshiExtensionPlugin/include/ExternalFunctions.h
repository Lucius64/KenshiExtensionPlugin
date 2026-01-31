#pragma once
#include <cstdint>
#include <string>

#include <boost/scoped_ptr.hpp>

#include <ogre/OgrePrerequisites.h>


enum WorldStateEnum;
enum StatsEnumerated;
enum itemType;
class AnimationData;
class AppearanceManager;
class GameData;
class UniqueNPCManager;
class FactionWarMgr;
class Faction;
class Character;
class GunClassPersonal;
class CampaignInstance;
class AreaSector;
class TownList;
class Platoon;
class ZoneManager;
class ZoneMap;
class CampaignData;
class hand;
class TownBase;
class Item;
class Town;
class AttachedEntity;
class AppearanceBase;
class AppearanceHuman;
class AnimsListsManager;
class RenderToTexture;
class PortraitManager;
class CLASS_02130220;
class iVector2;
class UtilityT;
class TerrainDecalsManager;
class ActivePlatoon;
class Building;
class BlueprintItem;
class DistantTown;
class Research;
class OptionsWindow;
template<typename T>
class lektor;
class ForgottenGUI;
class DatapanelGUI;
class DataPanelLine;
class DataPanelLine_CheckBox;
class DataPanelLine_SliderEditable;
class DataPanelLine_DropBox;
class LocaleInfoManager;
class SquadManagementScreen;
class AI;
class Blackboard;
class OrdersReceiver;
class AITaskSystem;
class ActionDeque;
class ProductionBuilding;
class FurnaceBuilding;
class Task_FillMachine;
class StorageBuilding;
class Inventory;
class PhysicsEntity;
class RobotLimbItem;
template<typename T>
class FitnessSelector;
class GameDataReference;
class GameDataContainer;
class FactionRelations;
class RangedCombatClass;
class AnimationClassBase;
class AnimationClass;
class InventorySection;
class InventoryLayout;
class AnimalInventoryLayout;
class InventoryGUI;
class InventorySectionGUI;
class SeveredLimbItem;
class CraftingBuilding;
class GameDataGroup;
class NewGameWindow;
class GunClass;
enum StatsEnumerated;

namespace MyGUI
{
	struct Colour;
	class Widget;
	class TextBox;
	class Window;
	class Button;
	class ScrollView;
	class ScrollBar;
	class ComboBox;
	class ItemBox;

	namespace types
	{
		template<typename T>
		struct TSize;
		template<typename T>
		struct TCoord;
	}
}

namespace boost
{
	namespace locale
	{
		template<typename CharType>
		class basic_message;
	}
}

namespace wraps
{
	class BaseLayout;
}

namespace KEP
{
	class CLASS_02133098
	{
	public:
		void* _0x0;
		TownList* townList;
		ZoneManager* zoneManager;
	};

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

		//placeholder name
		class OptionsAdvanced
		{
		public:
			float _cod;
			bool _ep;
			float _gdm;
			float _bs;
			float _nonm;
			float _rs;
			float _ps;
			float _ht;
			bool _banditsLootThePlayer;
			bool _enableAnimalHunger;
			bool _heal;
		};

		BaseXpCombat* _gBaseXpCombat;
		OptionsAdvanced* _optionsAdvanced;
		uint32_t* _playerMorphCounter;
		std::string* _KenshiFolder;
		UtilityT* _UtilityT;
		CLASS_02133098* _CLASS_02133098;
		CLASS_02130220* _CLASS_021322B0; // main window
		hand* _hand_01E395F8;
		std::string* _MainColorCode;
		std::string* _SecondarColorCode;
		std::string* _TitleColorCode;
		std::string* _BadColorCode;
		std::string* _BadBrightColorCode;
		std::string* _GoodColorCode;
		std::string* _GoodBrightColorCode;
		std::string* _GreyedColorCode;
		std::string* _GreyedBrightColorCode;
		std::string* _SpecialColorCode;
		MyGUI::Colour* _NameTagSelectedColor;
		MyGUI::Colour* _NameTagUnselectedColor;
		MyGUI::Colour* _NameTagStealthHiddenColor;
		MyGUI::Colour* _NameTagStealthAlmostSeenColor;
		MyGUI::Colour* _NameTagStealthSeenColor;
		MyGUI::Colour* _DialogueBubbleTextColor;
		ForgottenGUI* _KenshiGUI;
		LocaleInfoManager* _LocaleInfoManagerPtr;
		GameData* _TemporaryLimbInterface;
		MyGUI::types::TSize<int>* _InventorySlotSkinSize;

		uintptr_t _dropItemReturnAddress;

		ExternalGlobals();
		void init(unsigned int platform, const std::string& version, uintptr_t baseAddr);

	};

	class ExternalFunctions
	{
	public:
		Ogre::Entity* (*FUN_00071920)(Ogre::Entity*, const Ogre::SharedPtr<Ogre::Mesh>&); // Assign an existing mesh
		Ogre::Entity* (*FUN_00071A80)(const std::string&, Ogre::Entity*, GameData*, bool); // Duplicate mesh
		void (*FUN_0007B780)(AppearanceManager*, GameData*, float); // Init random appearance
		Ogre::Entity* (*FUN_0007C030)(AppearanceManager*, GameData*, Ogre::Entity*, bool, float, bool); // Create morph
		void (*FUN_008C4F20)(float&, float, float); // Gain skill (Except medic & robotics)
		GunClassPersonal* (*FUN_0043BDE0)(GunClassPersonal*, GameData*, float); // GunClassPersonal(GameData*, float)
		UniqueNPCManager& (*FUN_00354500)(void); // Get UniqueNPCManager singleton
		void (*FUN_00505D50)(UniqueNPCManager&, GameData*, const hand&); // Remove UniqueCharacterState
		bool (*FUN_00591720)(UniqueNPCManager&, GameData*); // Has not UniqueCharacterState
		WorldStateEnum (*FUN_005E7D60)(UniqueNPCManager&, GameData*); // Get character world state
		void (*FUN_009A7AA0)(UniqueNPCManager&); // Init UniqueCharacterState
		void (*FUN_009A93A0)(UniqueNPCManager&, GameData*, bool); // Load UniqueCharacterState
		bool (*FUN_009C4000)(CampaignData*); // Check world state
		void (*FUN_009C4170)(FactionWarMgr*, TownBase*, CampaignInstance*); // Create warcampaign squads
		void (*FUN_009C3280)(FactionWarMgr*, TownBase*, CampaignInstance*); // Create squad
		CampaignInstance* (*FUN_002834C0)(FactionWarMgr*, Platoon*); // Get CampaignInstance
		bool (*FUN_009C3510)(Platoon*, CampaignInstance*, TownBase*); // Participate in events
		lektor<TownBase*>* (*FUN_009429B0)(UtilityT*, const Ogre::Vector3&, lektor<TownBase*>&, int); // List of nearest towns
		AreaSector* (*FUN_008F47E0)(void*, const Ogre::Vector3&); // Get AreaSector
		bool (*FUN_008F4200)(AreaSector*, const Ogre::Vector3&, bool, Faction*, float, float); // Calculate spawn position
		void (*FUN_005375D0)(AppearanceHuman*, bool); // Set visibility of AttachedEntity
		void (*FUN_00531B70)(AppearanceBase*, AttachedEntity*); // Update hair texture
		void (*FUN_00536C00)(AppearanceBase*, AttachedEntity*); // Load AttachedEntity
		AnimsListsManager* (*FUN_000B70A0)(void); // Get AnimsListsManager singleton
		void (*FUN_005BC680)(AnimsListsManager*); // Init AnimsListsManager
		void (*FUN_0084A9C0)(RenderToTexture*, Character*, Ogre::SharedPtr<Ogre::Texture>, const PortraitManager&, const Ogre::Vector4&); // Render portrait
		void (*FUN_00845060)(RenderToTexture*, const iVector2&, float, float); // Set camera
		void (*FUN_00844C90)(Ogre::Entity* entity, const std::string& animName, float frame); // Set animation time position
		float(*FUN_008DBF80)(TerrainDecalsManager*, uint8_t, Ogre::Vector3&, Ogre::Vector3&, float, Ogre::ColourValue&, float); // 
		float(*FUN_008DC0D0)(TerrainDecalsManager*, Character*, Ogre::Vector3&, float, Ogre::ColourValue&);
		bool (*FUN_00958550)(ActivePlatoon*); // Has vendor list
		void* (*FUN_004FDE00)(void); // Shop trader singleton
		void (*FUN_0095A340)(void*, Building*, GameData*, Platoon*, bool, bool); // Refresh residents squad inventory
		BlueprintItem* (*FUN_002B7860)(BlueprintItem*, GameData*, GameData*, hand); // BlueprintItem(GameData*, GameData*, hand)
		void (*FUN_00371480)(Town*, GameData*); // Load town
		void (*FUN_009FD8D0)(Town*, GameData*); // Override town
		DistantTown* (*FUN_000D5FD0)(DistantTown*, Town*); // DistantTown(Town*)
		void (*FUN_00830b90)(Research*, GameData*); // Set BuildingUpgrades
		OptionsWindow* (*FUN_00406B90)(void); // Get OptionsWindow singleton
		void (*FUN_003FB250)(OptionsWindow*); // Open OptionsWindow
		void (*target_Ogre_Skeleton__refreshAnimationState)(Ogre::Skeleton*, Ogre::AnimationStateSet*);
		DataPanelLine* (*FUN_006FDE20)(DatapanelGUI*, const std::string&, const std::string&, int, bool, bool); // Create DataPanelLine
		DataPanelLine_CheckBox* (*FUN_006FE210)(DatapanelGUI*, const std::string&, bool&, int); // Create DataPanelLine_CheckBox
		DataPanelLine_SliderEditable* (*FUN_006FE880)(DatapanelGUI*, const std::string&, int, bool, float, float, float&); // Create DataPanelLine_SliderEditable
		DataPanelLine_DropBox* (*FUN_006FE2D0)(DatapanelGUI*, const std::string&, int, int&, bool, float); // Create DataPanelLine_DropBox
		DatapanelGUI* (*FUN_0073F4B0)(ForgottenGUI*, const std::string&, MyGUI::Widget*, bool); // Create DatapanelGUI
		void (*FUN_006F57E0)(DataPanelLine_SliderEditable*, int); // DataPanelLine_SliderEditable
		void (*FUN_006F6340)(DataPanelLine_DropBox*, const std::string&, int); // Add DropBox item
		void (*FUN_003F0120)(OptionsWindow*); // Init OptionsTab
		void (*FUN_003EC950)(OptionsWindow*); // Save OptionsHolder
		void (*FUN_0048EFA0)(SquadManagementScreen*, int); // dissmiss character
		void (*FUN_0048EA30)(SquadManagementScreen*);
		Blackboard* (*FUN_005065E0)(AI*); // Get Blackboard
		void (*FUN_002715E0)(Blackboard*, GameData*); // Set squad AI package
		void (*FUN_0050C720)(ActionDeque*); // Clear ActionDeque
		void (*FUN_005067F0)(OrdersReceiver*); // Delete all pjob
		void (*FUN_004AB930)(AITaskSystem*); // Clear AITaskSystem
		void (*FUN_00927900)(TownBase*, Faction*, ActivePlatoon*); // void TownBase::setFaction(Faction*, ActivePlatoon*)
		void (*FUN_0029E770)(ProductionBuilding*); // void ProductionBuilding::setupFromData()
		void (*FUN_0029F940)(FurnaceBuilding*); // void FurnaceBuilding::setupFromData()
		float (*FUN_002A5F00)(FurnaceBuilding*, Item*); //
		float (*FUN_002A5710)(FurnaceBuilding*, GameData*);
		void (*FUN_0029C810)(FurnaceBuilding*, lektor<GameData*>&);
		void (*FUN_00299750)(FurnaceBuilding*, DataPanelLine*);
		bool (*FUN_005A3B60)(AI*, hand&); // haveSomeResourcesFor
		int (*FUN_00340EB0)(Task_FillMachine*, StorageBuilding*, Inventory*);
		int (*FUN_00343720)(Task_FillMachine*, StorageBuilding*, Inventory*);
		int (*FUN_001A4FB0)(PhysicsEntity*, const char*); // Load scythe file
		RobotLimbItem* (*FUN_000CE290)(RobotLimbItem*, GameData*, GameData*, hand, int); // RobotLimbItem::RobotLimbItem(RobotLimbItem*, GameData*, GameData*, hand, int);
		void (*FUN_00954340)(FitnessSelector<uint32_t>&, GameData*);
		GameData* (*FUN_006BEAC0)(GameDataReference*, GameDataContainer*);
		void (*FUN_0052B1E0)(AppearanceHuman*); // muscular and body bulk
		AppearanceManager* (*FUN_0007F0B0)(AppearanceManager*); // AppearanceManager()
		AppearanceManager* (*FUN_0053E320)(); // Get AppearanceManager Singleton
		void (*FUN_0006F8F0)(AppearanceManager*, GameData*, Character*); // Set idle stance
		AnimationData* (*FUN_005B97F0)(AnimationData*, GameData*); // AnimationData(GameData*)
		PortraitManager* (*FUN_00413FE0)(PortraitManager*);
		void (*FUN_006B3460)(FactionRelations*, GameData*);
		void (*FUN_006B4A70)(FactionRelations*, GameData*, bool);
		void (*FUN_0051DA50)(RangedCombatClass*, const hand&, const Ogre::Vector3&, Character*);
		void (*FUN_005B7240)(AnimationClass*, AnimationData*, float, float);
		void (*FUN_005B7030)(AnimationClass*, AnimationData*, float, int, float);
		float (*FUN_005B3020)(AnimationClassBase*, AnimationData*);
		void (*FUN_005B33B0)(AnimationClassBase*, const Ogre::String&);
		void (*FUN_005B3380)(AnimationClassBase*, AnimationData*);
		AnimalInventoryLayout* (*FUN_00155350)(AnimalInventoryLayout*); // AnimalInventoryLayout::AnimalInventoryLayout()
		void (*FUN_0015D810)(wraps::BaseLayout*, MyGUI::TextBox**, const std::string&, bool, bool); // Get Widget <TextBox>
		void (*FUN_0011DDC0)(wraps::BaseLayout*, MyGUI::Widget**, const std::string&, bool, bool); // Get Widget <Widget>
		void (*FUN_0011CD50)(wraps::BaseLayout*, MyGUI::Button**, const std::string&, bool, bool); // Get Widget <Button>
		void (*FUN_00119C00)(wraps::BaseLayout*, MyGUI::Window**, const std::string&, bool, bool); // Get Widget <Window>
		void (*FUN_0014F530)(AnimalInventoryLayout*, InventoryGUI*, Ogre::map<std::string, InventorySectionGUI*>::type*, Inventory*); // Create GUI
		InventorySectionGUI*(*FUN_0014E390)(InventoryLayout*, InventorySection*); // Get InventorySectionGUI
		MyGUI::Widget* (*FUN_0070C0A0)(InventorySectionGUI*); // Get Widget
		void (*FUN_0070C0B0)(InventorySectionGUI*, bool); // setVisible
		void (*FUN_000CD830)(SeveredLimbItem*); // SeveredLimbItem::destroyPhysical
		void (*FUN_00A09840)(ZoneManager*, lektor<ZoneMap*>&); // 
		void (*FUN_000D8620)(DistantTown*, bool); // Update DistantTown
		std::string* (*FUN_000D6360)(DistantTown*, std::string*); // Get DistantMesh Filename
		void (*FUN_000D8310)(DistantTown*); // Create DistantMesh
		void (*FUN_004B0B60)(void*, MyGUI::ItemBox*, MyGUI::types::TCoord<int>&, bool); // ResearchItemList
		void (*FUN_002C9840)(void*, MyGUI::ItemBox*, MyGUI::types::TCoord<int>&, bool); // CraftItemList
		GameData* (*FUN_002BB270)(CraftingBuilding*, int);
		GameData* (*FUN_002CE000)(); // Get PlayerWeapons
		void (*FUN_002BBCE0)(CraftingBuilding*, lektor<GameDataGroup>&); // Get CraftItemList
		float (*FUN_002B8260)(CraftingBuilding*, GameData*); // Get WeaponLevel
		void (*FUN_0082E2E0)(Research*, lektor<GameDataGroup>&, itemType, itemType); // Get EnableItems
		void (*FUN_002CA350)(const Ogre::vector<GameDataReference>::type*, itemType, lektor<GameDataGroup>&); // Get EnableItems
		void (*FUN_00912CA0)(NewGameWindow*); // Set NewGameList

		hand(UniqueNPCManager::* FUN_009AFCA0)(GameData*); // Get UniqueCharacterHandle (thiscall & RVO)
		std::string(boost::locale::basic_message<char>::* FUN_000A9580)(void); // boost::locale::basic_message<char>::str

		ExternalFunctions();
		void init(unsigned int platform, const std::string& version, uintptr_t baseAddr);
	};

	extern boost::scoped_ptr<ExternalFunctions> externalFunctions;
	extern boost::scoped_ptr<ExternalGlobals> externalGlobals;
}