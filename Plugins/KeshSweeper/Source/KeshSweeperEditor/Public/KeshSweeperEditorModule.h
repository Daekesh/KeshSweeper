// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Templates/SharedPointer.h"

#define DEFAULT_MINEFIELD_WIDTH 20
#define DEFAULT_MINEFIELD_HEIGHT 20
#define DEFAULT_MINEFIELD_DIFFICULTY 4.f

class FKeshSweeperGameModel;
class FKeshSweeperGameView;
class FKeshSweeperGameController;

class FKeshSweeperEditorModule : public IModuleInterface, public TSharedFromThis< FKeshSweeperEditorModule >
{

public:

	static FKeshSweeperEditorModule& Get();
	static FKeshSweeperEditorModule* GetPtr();

	// Handle the commands
	static void OnToolbarButtonClicked();
	static bool CanClickToolbarButton();

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr< FKeshSweeperGameModel > GetModel() const { return GameModel; };
	TSharedPtr< FKeshSweeperGameView > GetView() const { return GameView; };
	TSharedPtr< FKeshSweeperGameController > GetController() const { return GameController; };

protected:

	TSharedPtr< FKeshSweeperGameModel > GameModel;
	TSharedPtr< FKeshSweeperGameView > GameView;
	TSharedPtr< FKeshSweeperGameController > GameController;

};
