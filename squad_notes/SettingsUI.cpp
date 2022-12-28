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
	ImGui::Checkbox("Keep unTracked/Leave players on Squad note Panel"s.c_str(), &settings.settings.keepUntrackedPlayer);

	ImGui::Checkbox("Only Track Raid Squads"s.c_str(), &settings.settings.trackRaidSquad);

	ImGui::Checkbox("Do NOT close window on ESC"s.c_str(), &settings.settings.disableEscClose);

	ImGui::Checkbox("Hide Unofficial Extras Message"s.c_str(), &settings.settings.hideExtrasMessage);

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Clear the cache and reload notes data for all players"s.c_str());

	ImGui::PopStyleVar();
}
