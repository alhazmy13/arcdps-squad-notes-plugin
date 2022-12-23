#include "global.h"

#include <map>
#include <mutex>
#include <vector>

#include "extension/IconLoader.h"
#include "SquadNotesUI.h"
#include "Player.h"
#include "resource.h"
#include "Settings.h"

#include "extension/arcdps_structs.h"

#include <imgui/imgui.h>

std::vector<std::string> trackedPlayers;
std::mutex trackedPlayersMutex;
std::map<std::string, Player> cachedPlayers;
std::mutex cachedPlayersMutex;
std::vector<std::string> instancePlayers;
std::mutex instancePlayersMutex;
std::string selfAccountName;
bool extrasLoaded;



/**
 * Do not load, when more than 10 players are in your squad, we are not interested in open world stuff
 */
void loadNoteSizeChecked(Player& player) {
	if (trackedPlayers.size() <= 10) {
		loadPlayerNote(player);
	}
}


void loadPlayerNote(Player& player) {
	if (player.status == LoadingStatus::NotLoaded && Settings::instance().settings.showSquadNotes) {
		player.loadNote();
	}
}

void updateNote(std::string username, std::string note) {
	for (auto& cachedPlayer : cachedPlayers) {
		Player& player = cachedPlayer.second;
		if (player.username == username) {
			player.note = note;
			Settings::instance().settings.notes[username] = note;
		}
	}
}

/**
 * lock `trackedPlayersMutex` and `instancePlayersMutex` before calling this
 */
void removePlayer(const std::string& username, AddedBy addedByToDelete) {
	// remove specific user
	auto pred = [&username, addedByToDelete](const std::string& player) {
		if (username == player) {
			if (addedByToDelete == AddedBy::Miscellaneous) {
				return true;
			}
			const auto& cachedPlayerIt = cachedPlayers.find(player);
			if (cachedPlayerIt != cachedPlayers.end()) {
				return cachedPlayerIt->second.addedBy == addedByToDelete;
			}
		}

		return false;
	};

	// actually remove from tracking
	const auto& trackedSub = std::ranges::remove_if(trackedPlayers, pred);
	trackedPlayers.erase(trackedSub.begin(), trackedSub.end());

	const auto& instanceSub = std::ranges::remove_if(instancePlayers, pred);
	instancePlayers.erase(instanceSub.begin(), instanceSub.end());
}

/**
 * lock `trackedPlayersMutex` and `instancePlayersMutex` before calling this
 */
bool addPlayerAll(const std::string& username) {
	addPlayerInstance(username);
	return addPlayerTracking(username);
}

/**
 * add to this-instance players
 * only add to tracking, if not already there
 *
 * lock `instancePlayersMutex` before calling this
 */
bool addPlayerInstance(const std::string& username) {
	if (std::ranges::find(instancePlayers, username) == instancePlayers.end()) {
		instancePlayers.emplace_back(username);
		return true;
	}
	return false;
}

/**
 * add to tracking
 * only add to tracking, if not already there
 * 
 * lock `trackedPlayersMutex` before calling this
 */
bool addPlayerTracking(const std::string& username) {
	if (std::ranges::find(trackedPlayers, username) == trackedPlayers.end()) {
		trackedPlayers.emplace_back(username);
		return true;
	}
	return false;
}

/**
 *
 * lock `cachedPlayersMutex` before calling this!
 */
void updateCommander(const std::string& commanderName) {
	for (auto& cachedPlayer : cachedPlayers) {
		Player& player = cachedPlayer.second;
		if (player.username == commanderName) {
			player.commander = true;
		} else {
			player.commander = false;
		}
	}
}

void GlobalObjects::UpdateArcExports() {
	uint64_t e6_result = ARC_EXPORT_E6();
	uint64_t e7_result = ARC_EXPORT_E7();

	ARC_HIDE_ALL = (e6_result & 0x01);
	ARC_PANEL_ALWAYS_DRAW = (e6_result & 0x02);
	ARC_MOVELOCK_ALTUI = (e6_result & 0x04);
	ARC_CLICKLOCK_ALTUI = (e6_result & 0x08);
	ARC_WINDOW_FASTCLOSE = (e6_result & 0x10);


	uint16_t* ra = (uint16_t*)&e7_result;
	if (ra) {
		ARC_GLOBAL_MOD1 = ra[0];
		ARC_GLOBAL_MOD2 = ra[1];
		ARC_GLOBAL_MOD_MULTI = ra[2];
	}
}

bool GlobalObjects::ModsPressed() {
	auto io = &ImGui::GetIO();

	return io->KeysDown[ARC_GLOBAL_MOD1] && io->KeysDown[ARC_GLOBAL_MOD2];
}

bool GlobalObjects::CanMoveWindows() {
	if (!ARC_MOVELOCK_ALTUI) {
		return true;
	} else {
		return ModsPressed();
	}
}
