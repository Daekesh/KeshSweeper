// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#include "KeshSweeperEditorModule.h"
#include "KeshSweeperCommands.h"
#include "KeshSweeperStyle.h"
#include "KeshSweeperGameModel.h"
#include "KeshSweeperGameView.h"
#include "KeshSweeperGameController.h"

#define LOCTEXT_NAMESPACE "FKeshSweeperEditorModule"

TSharedPtr< FKeshSweeperEditorModule > FKeshSweeperEditorModule::Plugin = nullptr;

void FKeshSweeperEditorModule::StartupModule()
{
	FKeshSweeperEditorModule::Plugin = MakeShareable( this );

	// Register slate style overrides
	FKeshSweeperStyle::Initialize();

	// Register commands
	FKeshSweeperCommands::Register();
}

void FKeshSweeperEditorModule::ShutdownModule()
{
	FKeshSweeperEditorModule::Plugin.Reset();

	FKeshSweeperStyle::Shutdown();
	FKeshSweeperCommands::Unregister();

	if ( GameController.IsValid() )
		GameController->Destruct();

	if ( GameView.IsValid() )
		GameView->Destruct();

	if ( GameModel.IsValid() )
		GameModel->Destruct();	
}

void FKeshSweeperEditorModule::OnToolbarButtonClicked()
{
	// Only instantiate these when we need them. 
	// Avoids wasting resources and creating tickable objects.
	if ( !GameController.IsValid() )
	{
		GameModel = MakeShareable( new FKeshSweeperGameModel( Plugin ) );
		GameView = MakeShareable( new FKeshSweeperGameView( Plugin ) );
		GameController = MakeShareable( new FKeshSweeperGameController( Plugin ) );

		GameModel->Init();
		GameView->Init();
		GameController->Init();
	}

	GameController->OnToolbarButtonClick();
}

bool FKeshSweeperEditorModule::CanClickToolbarButton()
{
	return true;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE( FKeshSweeperEditorModule, KeshSweeperEditor )
