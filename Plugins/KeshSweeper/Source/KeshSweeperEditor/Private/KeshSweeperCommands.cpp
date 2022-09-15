// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#include "KeshSweeperCommands.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "KeshSweeperEditorModule.h"
#include "LevelEditor.h"
#include "Styling/SlateStyle.h"

#define LOCTEXT_NAMESPACE "FKeshSweeperEditorModule"

FKeshSweeperCommands::FKeshSweeperCommands() 
	: TCommands< FKeshSweeperCommands > (
		TEXT( "KeshSweeperStyle" ),
		LOCTEXT( "KeshSweeperEditor", "Kesh Sweeper Editor" ),
		NAME_None,
		FKeshSweeperStyle::Get()->GetStyleSetName()
	)
{
}

void FKeshSweeperCommands::RegisterCommands()
{
	// Only register once
	if ( CommandList.IsValid() )
		return;

	// Assigns style based on the variable name
	UI_COMMAND( OpenWindow, "Kesh Sweeper", "Opens the Kesh Sweeper window", EUserInterfaceActionType::Button, FInputGesture() );

	// Start a scope
	{
		// Create command list and add it to the editor
		CommandList = MakeShareable( new FUICommandList );

		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );
		CommandList->Append( LevelEditorModule.GetGlobalLevelEditorActions() );

		CommandList->MapAction(
			OpenWindow,
			FExecuteAction::CreateStatic( &FKeshSweeperEditorModule::OnToolbarButtonClicked ),
			FCanExecuteAction::CreateStatic( &FKeshSweeperEditorModule::CanClickToolbarButton )
		);

		// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
		FToolMenuOwnerScoped OwnerScoped(this);

		{
			UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
			{
				FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
				{
					FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(OpenWindow));
					Entry.SetCommandList(CommandList);
				}
			}
		}
	}
	// Scope
}

#undef LOCTEXT_NAMESPACE
