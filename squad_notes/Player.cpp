#include "Player.h"

#include "Settings.h"


void Player::loadNote()
{
	if (Settings::instance().settings.notes.count(username)) {
		note = Settings::instance().settings.notes[username];
	}
}
