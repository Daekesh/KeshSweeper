// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "KeshSweeperStyle.h"
#include "SlateBasics.h"

class FKeshSweeperCommands : public TCommands< FKeshSweeperCommands >
{

public:
	
	FKeshSweeperCommands();

	// TCommand<> interface
	void RegisterCommands() override;
	// End of TCommand<> interface

public:

	TSharedPtr< FUICommandInfo > ToggleWindow;

};
