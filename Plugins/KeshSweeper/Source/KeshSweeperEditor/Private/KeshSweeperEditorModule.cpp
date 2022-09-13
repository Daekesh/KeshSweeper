// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#include "KeshSweeperEditorModule.h"
#include "KeshSweeperCommands.h"
#include "KeshSweeperStyle.h"
#include "KeshSweeperGameModel.h"
#include "KeshSweeperGameView.h"
#include "KeshSweeperGameController.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FKeshSweeperEditorModule"

FKeshSweeperEditorModule& FKeshSweeperEditorModule::Get()
{
	return FModuleManager::GetModuleChecked< FKeshSweeperEditorModule >( "KeshSweeperEditor" );
}

FKeshSweeperEditorModule* FKeshSweeperEditorModule::GetPtr()
{
	return static_cast< FKeshSweeperEditorModule* >( FModuleManager::Get().GetModule( "KeshSweeperEditor" ) );
}

void FKeshSweeperEditorModule::OnToolbarButtonClicked()
{
	FKeshSweeperEditorModule& Module = FKeshSweeperEditorModule::Get();

	// Only instantiate these when we need them. 
	// Avoids wasting resources and creating tickable objects.
	if ( !Module.GameController.IsValid() )
	{
		Module.GameModel = MakeShared< FKeshSweeperGameModel >();
		Module.GameView = MakeShared< FKeshSweeperGameView >();
		Module.GameController = MakeShared< FKeshSweeperGameController >();

		Module.GameModel->Init();
		Module.GameView->Init();
		Module.GameController->Init();
	}

	Module.GameController->OnToolbarButtonClick();
}

bool FKeshSweeperEditorModule::CanClickToolbarButton()
{
	return true;
}

void FKeshSweeperEditorModule::StartupModule()
{
	// Register slate style overrides
	FKeshSweeperStyle::Initialize();

	// Register commands
	FKeshSweeperCommands::Register();
}

void FKeshSweeperEditorModule::ShutdownModule()
{
	FKeshSweeperStyle::Shutdown();
	FKeshSweeperCommands::Unregister();

	if ( GameController.IsValid() )
		GameController->Destruct();

	if ( GameView.IsValid() )
		GameView->Destruct();

	if ( GameModel.IsValid() )
		GameModel->Destruct();	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE( FKeshSweeperEditorModule, KeshSweeperEditor )
