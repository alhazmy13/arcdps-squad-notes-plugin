#include "Settings.h"

#include "global.h"

#include "unofficial_extras/KeyBindHelper.h"

#include <fstream>


void Settings::load() {
	// according to standard, this constructor is completely thread-safe
	// read settings from file
	readFromFile();
}

void Settings::unload() {
	try {
		saveToFile();
	} catch (const std::exception& e) {
		// Some exception happened, i cannot do anything against it here :(
	}
}

void Settings::saveToFile() {
	// create json object
	auto json = nlohmann::json(settings);

	// open output file
	std::ofstream jsonFile("addons\\arcdps\\arcdps_squad_notes.json");

	// save json to file
	jsonFile << json;
}

void Settings::readFromFile() {
	try {
		// read a JSON file as stream
		std::ifstream jsonFile("addons\\arcdps\\arcdps_squad_notes.json");
		if (jsonFile.is_open()) {
			nlohmann::json json;

			// push stream into json object (this also parses it)
			jsonFile >> json;

			// get the object into the settings object
			json.get_to(settings);


		}
	} catch (const std::exception& e) {
		// some exception was thrown, all settings are reset!
	}
}
