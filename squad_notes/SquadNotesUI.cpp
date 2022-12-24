#include "SquadNotesUI.h"

#include "global.h"
#include "SquadNotesUIKeyBindComponent.h"
#include "SquadNotesUIPositioningComponent.h"
#include "Player.h"
#include "Settings.h"
#include "Log.h"
#include "extension/ExtensionTranslations.h"
#include "extension/IconLoader.h"
#include "extension/imgui_stdlib.h"
#include "extension/Localization.h"
#include "extension/Widgets.h"

#include <future>
#include <mutex>
#include <Windows.h>

using std::string_literals::operator ""s;

SquadNotesUI::SquadNotesUI() {
	CreateComponent<SquadNotesUIPositioningComponent>();
	CreateComponent<SquadNotesUIKeyBindComponent>();

	mTable = std::make_unique<SquadNotesUITable>(this, MainTableFlags_SubWindow);
}

bool& SquadNotesUI::GetOpenVar() {
	return Settings::instance().settings.showSquadNotes;
}

SizingPolicy& SquadNotesUI::getSizingPolicy() {
	return Settings::instance().settings.sizingPolicy;
}

bool& SquadNotesUI::getShowTitleBar() {
	return Settings::instance().settings.showHeader;
}

std::optional<std::string>& SquadNotesUI::getTitle() {
	return Settings::instance().settings.headerText;
}

const std::string& SquadNotesUI::getTitleDefault() {
	return mTitleDefault;
}

const std::string& SquadNotesUI::getWindowID() {
	return mWindowId;
}

std::optional<std::string>& SquadNotesUI::getAppearAsInOption() {
	return Settings::instance().settings.appearAsInOption;
}

const std::string& SquadNotesUI::getAppearAsInOptionDefault() {
	return mAppearAsInOptionDefault;
}

bool& SquadNotesUI::getShowBackground() {
	return Settings::instance().settings.showBackground;
}

bool& SquadNotesUI::GetShowScrollbar() {
	return Settings::instance().settings.showScrollbar;
}

std::optional<ImVec2>& SquadNotesUI::getPadding() {
	return Settings::instance().settings.windowPadding;
}

void SquadNotesUI::DrawContextMenu() {
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

	mTable->DrawColumnSetupMenu();

	ImGui::PopStyleVar();
}

void SquadNotesUI::DrawStyleSettingsSubMenu() {
	Settings& settings = Settings::instance();
	ImGui::Checkbox("Show controls"s.c_str(), &settings.settings.showControls);
	ImGui::Checkbox("Show Commander Tag"s.c_str(), &settings.settings.showCommander);


	ImGui::Separator();

	MainWindow::DrawStyleSettingsSubMenu();
}

void SquadNotesUI::DrawContent() {
	// lock the mutexes, before we access sensible data
	std::scoped_lock<std::mutex, std::mutex> lock(trackedPlayersMutex, cachedPlayersMutex);

	auto& settings = Settings::instance();


	/**
	 * Unofficial Extras message
	 */
	if (!extrasLoaded && !settings.settings.hideExtrasMessage) {
		ImGui::TextUnformatted("Unofficial extras plugin is not installed.\nInstall it to enable tracking of players on other instances."s.c_str());

		ImGui::SameLine();
		if (ImGui::Button(Localization::STranslate(ET_UpdateOpenPage).c_str())) {
			std::thread([] {
				ShellExecuteA(nullptr, nullptr, "https://github.com/Krappa322/arcdps_unofficial_extras_releases/releases/latest", nullptr, nullptr, SW_SHOW);
			}).detach();
		}

		ImGui::SameLine();
		if (ImGui::Button("X")) {
			extrasLoaded = true;
		}
	}

	/**
	* Controls
	*/
	if (settings.settings.showControls) {
		bool addPlayer = false;
		if (ImGui::InputText("##useradd", userAddBuf, sizeof userAddBuf, ImGuiInputTextFlags_EnterReturnsTrue)) {
			addPlayer = true;
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Accountname, or Charactername to search and add to the list"s.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Add"s.c_str())) {
			addPlayer = true;
		}

		if (addPlayer) {
			std::string username(userAddBuf);

			// only run when username is not empty
			if (!username.empty()) {
				const auto& existingPlayer = std::ranges::find_if(cachedPlayers, [&username](const auto& elem) -> bool {
					return elem.first == username || elem.second.characterName == username;
				});

				if (existingPlayer == cachedPlayers.end()) {
					// add new player
					trackedPlayers.emplace_back(username);

					const auto& tryEmplace = cachedPlayers.try_emplace(username, username, AddedBy::Manually);
					// This should always be `true`
					if (tryEmplace.second) {
						loadPlayerNote(tryEmplace.first->second);
					}
				} else {
					// player already exists
					// update username to actual accountname
					username = existingPlayer->first;
					
					// check of player is already tracked
					if (std::ranges::find(trackedPlayers, username) == trackedPlayers.end()) {
						// not yet tracked, add to tracking and update
						trackedPlayers.emplace_back(username);

						// set to Manually added
						existingPlayer->second.addedBy = AddedBy::Manually;
						existingPlayer->second.resetJoinedTime();
						loadPlayerNote(existingPlayer->second);
					} else {
						// user already exists and user is already tracked -> Try to load notes to override sizeCheck
						loadPlayerNote(existingPlayer->second);
					}
				}
				userAddBuf[0] = '\0';
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Clear"s.c_str())) {
			auto pred = [](const std::string& player) {
				const auto& cachedIt = cachedPlayers.find(player);
				if (cachedIt != cachedPlayers.end()) {
					return cachedIt->second.unTracked;
				}

				return false;
			};

			const auto& trackedSub = std::ranges::remove_if(trackedPlayers, pred);
			trackedPlayers.erase(trackedSub.begin(), trackedSub.end());

			const auto& instanceSub = std::ranges::remove_if(instancePlayers, pred);
			instancePlayers.erase(instanceSub.begin(), instanceSub.end());
			trackedPlayers = instancePlayers;
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Remove all manually/unTracked users"s.c_str());
		}

		
	}

	/**
	* TABLE
	*/
	mTable->Draw();
}
