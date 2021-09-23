// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#include "KeshSweeperEditorModule.h"
#include "KeshSweeperCommands.h"
#include "KeshSweeperStyle.h"
#include "KeshSweeperGameModel.h"
#include "KeshSweeperGameView.h"
#include "KeshSweeperGameController.h"

#define LOCTEXT_NAMESPACE "FKeshSweeperEditorModule"

void FKeshSweeperEditorModule::StartupModule()
{
	// Register slate style overrides
	FKeshSweeperStyle::Initialize();

	GameModel = MakeShareable( new FKeshSweeperGameModel( MakeShareable( this ) ) );
	GameView = MakeShareable( new FKeshSweeperGameView( MakeShareable( this ) ) );
	GameController = MakeShareable( new FKeshSweeperGameController( MakeShareable( this ) ) );

	GameModel->Init();
	GameView->Init();
	GameController->Init();

	// Register commands
	FKeshSweeperCommands::Register();

	if ( !CommandList.IsValid() )
	{
		CommandList = MakeShareable( new FUICommandList );

		{
			FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );
			CommandList->Append( LevelEditorModule.GetGlobalLevelEditorActions() );

			CommandList->MapAction(
				FKeshSweeperCommands::Get().ToggleWindow,
				FExecuteAction::CreateRaw( this, &FKeshSweeperEditorModule::HandleToggleWindowExecute ),
				FCanExecuteAction::CreateRaw( this, &FKeshSweeperEditorModule::HandleToggleWindowCanExecute )
			);

			struct Local
			{
				static void AddToolbarCommands( FToolBarBuilder& ToolbarBuilder )
				{
					ToolbarBuilder.AddToolBarButton( FKeshSweeperCommands::Get().ToggleWindow );
				}
			};

			TSharedRef< FExtender > ToolbarExtender( new FExtender() );

			ToolbarExtender->AddToolBarExtension(
				TEXT( "Game" ),
				EExtensionHook::After,
				CommandList.ToSharedRef(),
				FToolBarExtensionDelegate::CreateStatic( &Local::AddToolbarCommands )
			);

			LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender( ToolbarExtender );
		}
	}
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

void FKeshSweeperEditorModule::HandleToggleWindowExecute()
{
	if ( !GameController.IsValid() )
		return;

	GameController->InitiateGame();
}

bool FKeshSweeperEditorModule::HandleToggleWindowCanExecute()
{
	return true;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE( FKeshSweeperEditorModule, KeshSweeperEditor )
