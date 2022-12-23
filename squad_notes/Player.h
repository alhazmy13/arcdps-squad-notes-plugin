#pragma once

#include <optional>
#include <string>
#include <Windows.h>
#include <nlohmann/json.hpp>


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
	std::string errorMessage;
	AddedBy addedBy;
	bool commander = false;
	bool unTracked = false;
	SYSTEMTIME joinedTime;
	bool self = false;
	uint8_t subgroup = 0;

	void resetJoinedTime() {
		GetLocalTime(&joinedTime);
	}

	void loadNote();
};
