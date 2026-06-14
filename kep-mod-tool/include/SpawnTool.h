#pragma once
#include <string>
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreFastArray.h>

class GameData;
class DatapanelGUI;
class DataPanelLine;
class DataPanelLine_TextEditable;
class DataPanelLine_DropBox;

namespace KEP
{
	namespace tools
	{
		class SpawnTool
		{
		public:
			SpawnTool();
			int getCategory() const { return _category; }
			void setPanel(DatapanelGUI* panel) { _panel = panel; };
			void refresh();

		private:
			void _spawnSquad(DataPanelLine* line);
			void _addCharacter(DataPanelLine* line);
			void _giveItem(DataPanelLine* line);
			void _triggerCampaign(DataPanelLine* line);
			void _refreshInventory(DataPanelLine* line);
			void _overwriteRelation(DataPanelLine* line);
			void _changeUniqueNpcState(DataPanelLine* line);

			void _changeFactionSearchText(DataPanelLine* line);
			void _changeSquadSearchText(DataPanelLine* line);
			void _changeCharacterSearchText(DataPanelLine* line);
			void _changeItemSearchText(DataPanelLine* line);
			void _changeWeaponSearchText(DataPanelLine* line);
			void _selectItemList(DataPanelLine* line);
			void _selectItemCategoryList(DataPanelLine* line);
			void _selectItemSubcategoryList(DataPanelLine* line);
			void _selectWeaponCategoryList(DataPanelLine* line);
			void _changeCampaignSearchText(DataPanelLine* line);
			void _changeFromFactionSearchText(DataPanelLine* line);
			void _changeToFactionSearchText(DataPanelLine* line);
			void _changeUniqueNpcSearchText(DataPanelLine* line);
			void _changeUniformSearchText(DataPanelLine* line);
			void _changeColorSearchText(DataPanelLine* line);

			void _initFactionList();
			void _initSquadList();
			void _initCharacterList();
			void _initItemList();
			void _initWeaponList();
			void _initModelList();
			void _initCampaignList();
			void _initUniqueCharacters();
			void _initColorList();
			void _updateFactionList(const std::string& keyword);
			void _updateSquadList(const std::string& keyword);
			void _updateCharacterList(const std::string& keyword);
			void _updateItemList(const std::string& keyword);
			void _updateWeaponList(const std::string& keyword);
			void _updateModelList();
			void _updateItemSubcategorylList();
			void _updateCampaginList(const std::string& keyword);
			void _updateFromFactionList(const std::string& keyword);
			void _updateToFactionList(const std::string& keyword);
			void _updateUniqueNpcList(const std::string& keyword);
			void _updateUniformList(const std::string& keyword);
			void _updateColorList(const std::string& keyword);

			DatapanelGUI* _panel;
			const int _category;
			Ogre::FastArray<GameData*> _spawnFactionList;
			Ogre::FastArray<GameData*> _spawnSquadList;
			Ogre::FastArray<GameData*> _spawnCharacterList;
			Ogre::FastArray<GameData*> _spawnItemList;
			Ogre::FastArray<GameData*> _spawnWeaponList;
			Ogre::FastArray<GameData*> _spawnModelList;
			Ogre::FastArray<GameData*> _campaignList;
			Ogre::FastArray<GameData*> _uniqueCharacterList;
			std::set<GameData*> _uniquePrisonerList;
			Ogre::FastArray<GameData*> _itemColorList;
			int _gearLevel;
			int _selectedSpawnFaction;
			int _selectedSpawnSquad;
			int _selectedSpawnCharacter;
			int _selectedItem;
			int _selectedWeapon;
			int _selectedModel;
			int _searchItemCategory;
			int _searchItemSubCategory;
			int _searchWeaponCategory;
			float _quantity;
			int _selectedCampaign;
			float _animalAge;
			int _squadSpawnMethodType;
			int _selectedFromFaction;
			int _selectedToFaction;
			float _relation;
			int _selectedUniqueCharacter;
			int _uniqueState;
			int _selectedFactionUniform;
			int _selectedItemColor;
		};
	}
}
