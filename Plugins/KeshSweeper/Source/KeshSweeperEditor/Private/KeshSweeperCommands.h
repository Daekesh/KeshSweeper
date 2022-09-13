// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "KeshSweeperStyle.h"
#include "Framework/Commands/Commands.h"

class FKeshSweeperCommands : public TCommands< FKeshSweeperCommands >
{

public:

	FKeshSweeperCommands();

	// TCommand<> interface
	void RegisterCommands() override;
	// End of TCommand<> interface

	TSharedPtr< FUICommandList > CommandList;
	TSharedPtr< FUICommandInfo > OpenWindow;

};
