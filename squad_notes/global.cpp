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
#include "Log.h"
#include <imgui/imgui.h>

std::map<std::string, Player> trackedPlayers;
std::mutex trackedPlayersMutex;
//std::map<std::string, Player> cachedPlayers;
//std::mutex cachedPlayersMutex;
std::vector<std::string> instancePlayers;
std::mutex instancePlayersMutex;
std::string selfAccountName;
bool extrasLoaded;



/**
 * Do not load, when more than 10 players are in your squad, we are not interested in open world stuff
 */
void loadNoteSizeChecked(Player& player) {
	if (Settings::instance().settings.trackRaidSquad) {
		if (getTrackListSize() <  10) {
			loadPlayerNote(player);
		}
		return;
	}
	loadPlayerNote(player);
}

int getTrackListSize() {
	int size = 0;
	for (auto& cachedPlayer : trackedPlayers) {
		Player& player = cachedPlayer.second;
		if (!player.unTracked) size++;
	}
	return size;
}

void loadPlayerNote(Player& player) {
	if (Settings::instance().settings.showSquadNotes) {
		player.loadNote();
	}
}

void updateNote(std::string username, std::string note) {
	for (auto& cachedPlayer : trackedPlayers) {
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
	Log::instance().Logger("removePlayer","Start Remove User = " + username);
	auto pred = [&username, addedByToDelete](const std::string& player) {
		if (Settings::instance().settings.keepUntrackedPlayer) return false;
		if (username == player) {
			if (addedByToDelete == AddedBy::Miscellaneous) {
				return true;
			}
			const auto& cachedPlayerIt = trackedPlayers.find(player);
			if (cachedPlayerIt != trackedPlayers.end()) {
				return cachedPlayerIt->second.addedBy == addedByToDelete;
			}
		}

		return false;
	};

	// actually remove from tracking
	//const auto& trackedSub = std::ranges::remove_if(trackedPlayers, pred);
	//trackedPlayers.erase(trackedSub.begin(), trackedSub.end());

	const auto& instanceSub = std::ranges::remove_if(instancePlayers, pred);
	instancePlayers.erase(instanceSub.begin(), instanceSub.end());

	const std::string keepUntracked = Settings::instance().settings.keepUntrackedPlayer ? "true" : "false";
	Log::instance().Logger("removePlayer", "keepUntrackedPlayer = " + keepUntracked);
	if (Settings::instance().settings.keepUntrackedPlayer) {
		Log::instance().Logger("removePlayer", "Start Update Removed User = " + username);
		const auto& actuaPlayer = trackedPlayers.find(username);
		if (actuaPlayer != trackedPlayers.end()) {
			actuaPlayer->second.unTracked = true;
			Log::instance().Logger("removePlayer", "Start Removed User updated = " + username);
		}
	}
}

/**
 * lock `trackedPlayersMutex` and `instancePlayersMutex` before calling this
 */
bool addPlayerAll(const std::string& username) {
	if (Settings::instance().settings.trackRaidSquad) {
		if (getTrackListSize() < 10) {
			return addPlayerInstance(username);
		}
		return false;
	}
	return addPlayerInstance(username);
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
 *
 * lock `cachedPlayersMutex` before calling this!
 */
void updateCommander(const std::string& commanderName) {
	for (auto& cachedPlayer : trackedPlayers) {
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
