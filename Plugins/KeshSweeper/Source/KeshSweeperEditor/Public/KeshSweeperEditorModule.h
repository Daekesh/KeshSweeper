// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#define DEFAULT_MINEFIELD_WIDTH 20
#define DEFAULT_MINEFIELD_HEIGHT 20
#define DEFAULT_MINEFIELD_DIFFICULTY 4.f

class FKeshSweeperEditorModule : public IModuleInterface
{

protected:

	TSharedPtr< class FKeshSweeperGameModel > GameModel;
	TSharedPtr< class FKeshSweeperGameView > GameView;
	TSharedPtr< class FKeshSweeperGameController > GameController;

public:

	// I feel this is a bit hacky, but meh!
	static TSharedPtr< FKeshSweeperEditorModule > Plugin;

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// Handle the commands
	void OnToolbarButtonClicked();
	bool CanClickToolbarButton();

	TSharedPtr< class FKeshSweeperGameModel > GetModel() const { return GameModel; };
	TSharedPtr< class FKeshSweeperGameView > GetView() const { return GameView; };
	TSharedPtr< class FKeshSweeperGameController > GetController() const { return GameController; };

};
