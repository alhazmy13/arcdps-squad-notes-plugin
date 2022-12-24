#include "SquadNotesUITable.h"

#include "Settings.h"
#include "Player.h"
#include "resource.h"

template<bool Linked>
bool SquadNotesUITable::drawRow(TableColumnIdx pFirstColumnIndex, const Player& pPlayer, bool pTotal, bool pTotalText) {
	NextRow();

	bool accountNameEnabled = true;
	for (auto i = 0; i < mTable.DisplayOrderToIndex.size(); ++i) {
		// the most left of 1 or 2 (username and charname) which is also enabled
		if (mTable.DisplayOrderToIndex[i] == 1 && mTable.EnabledMaskByDisplayOrder.test(i)) {
			accountNameEnabled = true;
			break;
		}
		if (mTable.DisplayOrderToIndex[i] == 2 && mTable.EnabledMaskByDisplayOrder.test(i)) {
			accountNameEnabled = false;
			break;
		}
	}

	bool open = false;

	for (const MainTableColumn& column : COLUMN_SETUP) {
		if (NextColumn()) {
			const bool first = pFirstColumnIndex == GetColumnIndex();

			if (column.UserId == JOIN_TIME_ID) {
				SYSTEMTIME joinedTime = pPlayer.joinedTime;
				drawTextColumn<true>(open, std::format("{:02d}:{:02d}:{:02d}", joinedTime.wHour, joinedTime.wMinute, joinedTime.wSecond), pPlayer.username, pPlayer.unTracked,
					first, false);

				continue;
			}
			if (column.UserId == ACCOUNT_NAME_ID) {
				drawTextColumn<true>(open, pPlayer.username, pPlayer.username, pPlayer.unTracked, first, pPlayer.commander && accountNameEnabled);
				continue;
			}
			if (column.UserId == CHARACTER_NAME_ID) {
				drawTextColumn<true>(open, pPlayer.characterName, pPlayer.username, pPlayer.unTracked, first, pPlayer.commander && !accountNameEnabled);
				continue;
			}

			if (column.UserId == NOTES_ID) {
				char customNote[1024];
				strcpy_s(customNote, pPlayer.note.c_str());
				ImGui::SetNextItemWidth(300);
				if (ImGui::InputText(("##Notes" + pPlayer.username).c_str(), customNote, sizeof customNote, ImGuiInputTextFlags_EnterReturnsTrue)) {
					updateNote(pPlayer.username, customNote);
				}
				continue;
			}

			if (column.UserId == SUBGROUP_ID) {
				// subgroups are zero based and ui is one based
				drawTextColumn<true>(open, std::to_string(pPlayer.subgroup + 1), pPlayer.username, pPlayer.unTracked, first, false);
				continue;
			}

		}
	}

	return open;
}

namespace {
	std::optional<size_t> COMMANDER_TAG_TEXTURE;
}
template<bool AlignmentActive>
void SquadNotesUITable::drawTextColumn(bool& pOpen, const std::string& pText, const std::string& pUsername, const bool& unTracked, bool pTreeNode, bool
	pIsCommander) {
	if (pIsCommander && Settings::instance().settings.showCommander) {
		float size = ImGui::GetFontSize();
		ImGui::Image(GET_TEXTURE_CUSTOM(COMMANDER_TAG_TEXTURE, ID_Commander_White), ImVec2(size, size));
		ImGui::SameLine();
	}
	if (unTracked) {
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(172, 89, 89, 255));
	}
	if constexpr (AlignmentActive) {
		AlignedTextColumn(pText);
	}
	else {
		ImGui::TextUnformatted(pText.c_str());
	}
	if (unTracked) {
		ImGui::PopStyleColor();
	}
}


void SquadNotesUITable::DrawRows(TableColumnIdx pFirstColumnIndex) {
	const auto& settingsObject = Settings::instance().settings;

	// List of all players
	for (const std::string& trackedPlayer : trackedPlayers) {
		const Player& player = cachedPlayers.at(trackedPlayer);

		bool open = drawRow(pFirstColumnIndex, player, true);
		if (open) {
			drawRow<true>(pFirstColumnIndex, player, false);

			ImGui::TreePop();
		}

		EndMaxHeightRow();

	}
}

void SquadNotesUITable::Sort(const ImGuiTableColumnSortSpecs* mColumnSortSpecs) {
	const bool descend = mColumnSortSpecs->SortDirection == ImGuiSortDirection_Descending;

	if (mColumnSortSpecs->ColumnUserID == JOIN_TIME_ID) {
		std::ranges::sort(trackedPlayers, [descend](const std::string& playerAName, const std::string& playerBName) -> bool {
			const SYSTEMTIME& joinedTimeA = cachedPlayers.at(playerAName).joinedTime;
		const SYSTEMTIME& joinedTimeB = cachedPlayers.at(playerBName).joinedTime;

		auto tiedTimeA = std::tie(joinedTimeA.wYear, joinedTimeA.wMonth, joinedTimeA.wDay, joinedTimeA.wHour, joinedTimeA.wMinute, joinedTimeA.wSecond);
		auto tiedTimeB = std::tie(joinedTimeB.wYear, joinedTimeB.wMonth, joinedTimeB.wDay, joinedTimeB.wHour, joinedTimeB.wMinute, joinedTimeB.wSecond);

		if (descend) {
			return tiedTimeA < tiedTimeB;
		}
		return tiedTimeA > tiedTimeB;
			});
		return;
	}
	if (mColumnSortSpecs->ColumnUserID == ACCOUNT_NAME_ID) {
		// sort by account name. Account name is the value we used in trackedPlayers, so nothing more to do
		std::ranges::sort(trackedPlayers, [descend](std::string playerAName, std::string playerBName) {
			std::ranges::transform(playerAName, playerAName.begin(), [](unsigned char c) { return std::tolower(c); });
		std::ranges::transform(playerBName, playerBName.begin(), [](unsigned char c) { return std::tolower(c); });

		if (descend) {
			return playerAName < playerBName;
		}
		return playerAName > playerBName;
			});
		return;
	}
	if (mColumnSortSpecs->ColumnUserID == CHARACTER_NAME_ID) {
		// sort by character name
		std::ranges::sort(trackedPlayers, [descend](const std::string& playerAName, const std::string& playerBName) {
			std::string playerAChar = cachedPlayers.at(playerAName).characterName;
		std::string playerBChar = cachedPlayers.at(playerBName).characterName;

		std::ranges::transform(playerAChar, playerAChar.begin(), [](unsigned char c) { return std::tolower(c); });
		std::ranges::transform(playerBChar, playerBChar.begin(), [](unsigned char c) { return std::tolower(c); });

		if (descend) {
			return playerAChar < playerBChar;
		}
		return playerAChar > playerBChar;
			});
		return;
	}

	if (mColumnSortSpecs->ColumnUserID == SUBGROUP_ID) {
		// sort by subgroup
		std::ranges::sort(trackedPlayers, [descend](const std::string& playerAName, const std::string& playerBName) {
			uint8_t playerAGroup = cachedPlayers.at(playerAName).subgroup;
		uint8_t playerBGroup = cachedPlayers.at(playerBName).subgroup;

		if (descend) {
			return playerAGroup > playerBGroup;
		}
		return playerAGroup < playerBGroup;
			});
		return;
	}

}

Alignment& SquadNotesUITable::getAlignment() {
	return Settings::instance().settings.alignment;
}

Alignment& SquadNotesUITable::getHeaderAlignment() {
	return Settings::instance().settings.headerAlignment;
}

std::string SquadNotesUITable::getTableId() {
	return "SquadNotes";
}

int& SquadNotesUITable::getMaxDisplayed() {
	return Settings::instance().settings.maxDisplayed;
}

bool& SquadNotesUITable::getShowAlternatingBackground() {
	return Settings::instance().settings.showAlternatingRowBackground;
}

SquadNotesUITable::TableSettings& SquadNotesUITable::getTableSettings() {
	return Settings::instance().settings.tableSettings;
}

bool& SquadNotesUITable::getHighlightHoveredRows() {
	return Settings::instance().settings.highlightHoveredRows;
}



bool& SquadNotesUITable::getCustomColumnsActive() {
	return Settings::instance().settings.showBasedOnMap;
}

bool& SquadNotesUITable::getShowHeaderAsText() {
	return Settings::instance().settings.showHeaderText;
}

const char* SquadNotesUITable::getCategoryName(const std::string& pCat) {
	return "";
}