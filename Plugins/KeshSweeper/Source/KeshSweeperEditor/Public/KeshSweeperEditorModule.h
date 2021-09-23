// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "LevelEditor.h"

class FKeshSweeperEditorModule : public IModuleInterface
{

protected:

	TSharedPtr< FUICommandList > CommandList;
	TSharedPtr< class FKeshSweeperGameModel > GameModel;
	TSharedPtr< class FKeshSweeperGameView > GameView;
	TSharedPtr< class FKeshSweeperGameController > GameController;

public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// Handle the commands
	void HandleToggleWindowExecute();
	bool HandleToggleWindowCanExecute();

	TSharedPtr< class FKeshSweeperGameModel > GetModel() { return GameModel; };
	TSharedPtr< class FKeshSweeperGameView > GetView() { return GameView; };
	TSharedPtr< class FKeshSweeperGameController > GetController() { return GameController; };

};
