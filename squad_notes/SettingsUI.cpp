#include "SettingsUI.h"

#include <Windows.h>

#include "Player.h"
#include "Settings.h"
#include "global.h"

#include "extension/KeyBindHandler.h"
#include "extension/KeyInput.h"

#include <imgui/imgui.h>
#include "extension/Widgets.h"

void SettingsUI::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0.f, 0.f});

	Settings& settings = Settings::instance();


	// Setting to select, which key is used to open the squad notes menu (will also close it)
	KeyBinds::Modifier arcdpsModifier = KeyBindHandler::GetArcdpsModifier();
	KeyBinds::Key oldKey = settings.settings.windowKey;
	if (ImGuiEx::KeyCodeInput(Localization::STranslate(ET_Shortcut).c_str(), settings.settings.windowKey,
							  Language::English, GlobalObjects::CURRENT_HKL,
	                          ImGuiEx::KeyCodeInputFlags_FixedModifier, arcdpsModifier)) {
		KeyBindHandler::instance().UpdateKeys(oldKey, settings.settings.windowKey);
	}

	ImGui::Checkbox("Do NOT close window on ESC"s.c_str(), &settings.settings.disableEscClose);

	ImGui::Checkbox("Hide Unofficial Extras Message"s.c_str(), &settings.settings.hideExtrasMessage);

	if (ImGui::Button("Clear Cache"s.c_str())) {
		std::scoped_lock<std::mutex, std::mutex> guard(cachedPlayersMutex, trackedPlayersMutex);

		// get all accountnames and charnames
		std::list<Player> usersToKeep;
		for (std::string trackedPlayer : trackedPlayers) {
			const Player& player = cachedPlayers.at(trackedPlayer);
			usersToKeep.emplace_back(player.username, player.addedBy, player.self, player.characterName, player.id);
		}

		// clear the cache
		cachedPlayers.clear();
		settings.settings.notes.clear();
		// refill the cache with only tracked players
		for (const Player& player : usersToKeep) {
			const auto& tryEmplace = cachedPlayers.try_emplace(player.username, player.username, player.addedBy,
			                                                   player.self, player.characterName, player.id);

			// load kp.me data if less than 10 people tracked
			loadNoteSizeChecked(tryEmplace.first->second);
		}
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Clear the cache and reload notes data for all players"s.c_str());

	ImGui::PopStyleVar();
}
