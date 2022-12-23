#pragma once

#include "global.h"
#include "Player.h"
#include "resource.h"

#include "extension/IconLoader.h"
#include "extension/Windows/MainTable.h"

constexpr ImU32 JOIN_TIME_ID = 32;
constexpr ImU32 ACCOUNT_NAME_ID = 29;
constexpr ImU32 CHARACTER_NAME_ID = 30;
constexpr ImU32 NOTES_ID = 31;
constexpr ImU32 SUBGROUP_ID = 64;


using std::string_literals::operator ""s;

#define GET_TEXTURE_CUSTOM(optional, id) \
	std::invoke([] { \
		auto& iconLoader = IconLoader::instance(); \
		if (!optional) \
			optional = iconLoader.LoadTexture(id); \
		return iconLoader.GetTexture(optional.value()); \
	})

// the index is used for the order 
static const std::vector<MainTableColumn> COLUMN_SETUP {
	// general stuff
	{JOIN_TIME_ID, [] {return "#"s;}, []{return nullptr;}, "0", true},
	{ACCOUNT_NAME_ID, [] {return "Account"s;}, [] {return nullptr;}, "0", true},
	{CHARACTER_NAME_ID, [] {return "Character"s;}, [] {return nullptr;}, "0", true},
	{SUBGROUP_ID, [] {return "Groupe"s;}, [] {return nullptr;}, "0", true},
	{NOTES_ID, [] {return "Notes"s;}, [] {return nullptr;}, "0", true},
};



class SquadNotesUITable : public MainTable<> {
public:
	SquadNotesUITable(MainWindow* pMainWindow, MainTableFlags pFlags)
		: MainTable<>(COLUMN_SETUP, pMainWindow, pFlags) {}

protected:
	void DrawRows(TableColumnIdx pFirstColumnIndex) override;
	void Sort(const ImGuiTableColumnSortSpecs* mColumnSortSpecs) override;

	Alignment& getAlignment() override;
	Alignment& getHeaderAlignment() override;
	std::string getTableId() override;
	int& getMaxDisplayed() override;
	bool& getShowAlternatingBackground() override;
	TableSettings& getTableSettings() override;
	bool& getHighlightHoveredRows() override;
	const char* getCategoryName(const std::string& pCat) override;
	bool getCustomColumnsFeatureActive() override { return true; }
	bool& getCustomColumnsActive() override;
	int getCustomColumnsFirstColumn() override { return 5; }
	bool& getShowHeaderAsText() override;

private:
	template<bool Linked = false>
	bool drawRow(TableColumnIdx pFirstColumnIndex, const Player& pPlayer, bool pTotal = false, bool pTotalText = false);

	template<bool AlignmentActive = false>
	void drawTextColumn(bool& pOpen, const std::string& pText, const std::string& pUsername, const std::atomic<LoadingStatus>& pStatus, bool pTreeNode, bool pIsCommander);

};
