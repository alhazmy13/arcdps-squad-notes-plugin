#pragma once

#include "global.h"
#include "Settings.h"

#include "extension/Windows/KeyBindComponent.h"

class SquadNotesUIKeyBindComponent : public KeyBindComponent {
public:
	explicit SquadNotesUIKeyBindComponent(MainWindow* pMainWindow)
		: KeyBindComponent(pMainWindow) {}

protected:
	KeyBinds::Key& getKeyBind() override;
	HKL getCurrentHKL() override;
	bool getKeyBindSwitch() override;
	bool getCloseWithEsc() override;
	bool getCloseWithEscActive() override;
};
