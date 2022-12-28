#pragma once

#include <map>
#include <mutex>
#include <vector>
#include <d3d9.h>

#include "extension/UpdateChecker.h"
#include "extension/UpdateCheckerBase.h"
#include "unofficial_extras/Definitions.h"

constexpr auto SQUAD_NOTES_PLUGIN_NAME = "Squad Notes";

enum class AddedBy;
class Player;
class SquadNotesUI;

extern std::map<std::string, Player> trackedPlayers;
extern std::mutex trackedPlayersMutex;
//extern std::map<std::string, Player> cachedPlayers;
//extern std::mutex cachedPlayersMutex;
extern std::vector<std::string> instancePlayers;
extern std::mutex instancePlayersMutex;
extern HMODULE self_dll;
extern std::string selfAccountName;
extern bool extrasLoaded;

void loadNoteSizeChecked(Player& player);
void loadPlayerNote(Player& player);
void updateNote(std::string username, std::string note);

void removePlayer(const std::string& username, AddedBy addedByToDelete);
int getTrackListSize();
bool addPlayerAll(const std::string& username);
bool addPlayerInstance(const std::string& username);
void updateCommander(const std::string& commanderName);

class GlobalObjects {
public:
	// Updating myself stuff
	static inline std::unique_ptr<UpdateCheckerBase::UpdateState> UPDATE_STATE = nullptr;

	// arc keyboard modifier
	static inline DWORD ARC_GLOBAL_MOD1 = 0;
	static inline DWORD ARC_GLOBAL_MOD2 = 0;
	static inline DWORD ARC_GLOBAL_MOD_MULTI = 0;

	// Arc export Cache
	static inline bool ARC_HIDE_ALL = false;
	static inline bool ARC_PANEL_ALWAYS_DRAW = false;
	static inline bool ARC_MOVELOCK_ALTUI = false;
	static inline bool ARC_CLICKLOCK_ALTUI = false;
	static inline bool ARC_WINDOW_FASTCLOSE = false;

	// Arc helper functions
	static void UpdateArcExports();
	static bool ModsPressed();
	static bool CanMoveWindows();

	// other
	static inline HKL CURRENT_HKL;
};
