#include "SquadNotesUIKeyBindComponent.h"

KeyBinds::Key& SquadNotesUIKeyBindComponent::getKeyBind() {
	return Settings::instance().settings.windowKey;
}

HKL SquadNotesUIKeyBindComponent::getCurrentHKL() {
	return GlobalObjects::CURRENT_HKL;
}


bool SquadNotesUIKeyBindComponent::getKeyBindSwitch() {
	GlobalObjects::UpdateArcExports();
	return !GlobalObjects::ARC_HIDE_ALL;
}

bool SquadNotesUIKeyBindComponent::getCloseWithEsc() {
	GlobalObjects::UpdateArcExports();
	return !Settings::instance().settings.disableEscClose && GlobalObjects::ARC_WINDOW_FASTCLOSE;
}

bool SquadNotesUIKeyBindComponent::getCloseWithEscActive() {
	return true;
}
