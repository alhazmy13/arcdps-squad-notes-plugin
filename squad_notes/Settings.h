#pragma once

#include "SquadNotesUITable.h"

#include "extension/arcdps_structs.h"
#include "extension/nlohmannJsonExtension.h"
#include "extension/Singleton.h"

#include <imgui/imgui.h>

#include "unofficial_extras/Definitions.h"
#include "unofficial_extras/KeyBindStructs.h"

#include <map>

#include <nlohmann/json.hpp>

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_NON_THROWING(ImVec2, x, y)

struct SettingsKey : KeyBinds::Key {
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(SettingsKey, DeviceType, Code, Modifier)
};

inline void to_json(nlohmann::json& nlohmann_json_j, const SquadNotesUITable::TableColumnSettings& nlohmann_json_t) {
	nlohmann_json_j["WidthOrWeight"] = nlohmann_json_t.WidthOrWeight;
	nlohmann_json_j["UserID"] = nlohmann_json_t.UserID;
	nlohmann_json_j["DisplayOrder"] = nlohmann_json_t.DisplayOrder;
	nlohmann_json_j["SortOrder"] = nlohmann_json_t.SortOrder;
	nlohmann_json_j["SortDirection"] = nlohmann_json_t.SortDirection;
	nlohmann_json_j["IsEnabled"] = nlohmann_json_t.IsEnabled;
	nlohmann_json_j["IsStretch"] = nlohmann_json_t.IsStretch;
}

inline void from_json(const nlohmann::json& nlohmann_json_j, SquadNotesUITable::TableColumnSettings& nlohmann_json_t) {
	nlohmann_json_j.at("WidthOrWeight").get_to(nlohmann_json_t.WidthOrWeight);
	nlohmann_json_j.at("UserID").get_to(nlohmann_json_t.UserID);
	nlohmann_json_j.at("DisplayOrder").get_to(nlohmann_json_t.DisplayOrder);
	nlohmann_json_j.at("SortOrder").get_to(nlohmann_json_t.SortOrder);
	nlohmann_json_t.SortDirection = nlohmann_json_j.at("SortDirection").get<ImU8>();
	nlohmann_json_t.IsEnabled = nlohmann_json_j.at("IsEnabled").get<ImU8>();
	nlohmann_json_t.IsStretch = nlohmann_json_j.at("IsStretch").get<ImU8>();
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SquadNotesUITable::TableSettings, Version, SaveFlags, RefScale, Columns)

class Settings : public Singleton<Settings> {
public:
	struct SettingsObject {
		uint32_t version = 1;
		SettingsKey windowKey{KeyBinds::DeviceType::Keyboard, static_cast<int32_t>(KeyBinds::KeyCode::N), 0};
		bool showSquadNotes = false;
		bool disableEscClose = false;
		Alignment alignment = Alignment::Left;
		Alignment headerAlignment = Alignment::Left;
		bool showControls = true;
		bool showHeaderText = false; // this is for the header in the table
		bool showHeader = true;
		bool showBackground = true;
		bool showScrollbar = true;
		Position position = Position::Manual;
		CornerPosition cornerPosition = CornerPosition::TopLeft;
		ImVec2 cornerVector;
		CornerPosition anchorPanelCornerPosition = CornerPosition::TopLeft;
		CornerPosition selfPanelCornerPosition = CornerPosition::TopLeft;
		ImGuiID fromWindowID = 0;
		bool showCommander = false;
		bool hideExtrasMessage = false;
		SizingPolicy sizingPolicy = SizingPolicy::SizeToContent;
		std::optional<std::string> headerText;
		std::optional<std::string> appearAsInOption;
		std::optional<ImVec2> windowPadding;
		int maxDisplayed = 0;
		bool showAlternatingRowBackground = true;
		bool highlightHoveredRows = true;
		SquadNotesUITable::TableSettings tableSettings;
		bool showBasedOnMap = false;
		bool keepUntrackedPlayer = true;
		bool trackRaidSquad = true;
		std::map<std::string, std::string> notes;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE_NON_THROWING(SettingsObject, version, windowKey,
			showSquadNotes, notes,disableEscClose,
		                                            alignment, headerAlignment, showControls,
		                                            showHeader, position, keepUntrackedPlayer,
		                                            cornerPosition, cornerVector, anchorPanelCornerPosition,
		                                            selfPanelCornerPosition, fromWindowID, trackRaidSquad,
		                                            showCommander, hideExtrasMessage, sizingPolicy,
		                                            headerText, appearAsInOption, showBackground, showScrollbar,
		                                            windowPadding, maxDisplayed, showAlternatingRowBackground,
		                                            highlightHoveredRows, tableSettings, showBasedOnMap)
	};

	Settings() = default;

	void load();
	void unload();

	SettingsObject settings;


	// delete copy/move
	Settings(const Settings& other) = delete;
	Settings(Settings&& other) noexcept = delete;
	Settings& operator=(const Settings& other) = delete;
	Settings& operator=(Settings&& other) noexcept = delete;
private:
	void saveToFile();
	void readFromFile();
};
