#pragma once

#include <optional>
#include <string>
#include <Windows.h>
#include <nlohmann/json.hpp>

enum class LoadingStatus {
	// loading was not yet tried
	NotLoaded,
	// User has no kp.me account or account is private
	NoDataAvailable,
	// Successful, all data available to use
	Loaded,
	// Data is currently bering loaded by charactername
	LoadingByChar,
	// Data was loaded by a linked account name
	LoadedByLinked,
};

enum class AddedBy {
	Manually,
	Arcdps,
	Extras,
	Miscellaneous,
};

class Player {
public:
	Player(std::string username, AddedBy addedBy, bool self = false, std::string characterName = "", uintptr_t id = 0)
		: username(std::move(username)),
		  characterName(std::move(characterName)),
		  id(id),
		  addedBy(addedBy),
		  self(self) {
		resetJoinedTime();
	}

	Player() = default;

	uintptr_t id;
	std::string username;
	std::string note;
	std::string characterName;
	std::atomic<LoadingStatus> status{LoadingStatus::NotLoaded};
	std::string errorMessage;
	AddedBy addedBy;
	bool commander = false;
	SYSTEMTIME joinedTime;
	bool self = false;
	uint8_t subgroup = 0;

	void resetJoinedTime() {
		GetLocalTime(&joinedTime);
	}

	void loadNote();
};
