// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#include "KeshSweeperCommands.h"
#include "KeshSweeperEditorModule.h"
#include "LevelEditor.h"

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
	if ( !FKeshSweeperEditorModule::Plugin.IsValid() )
		return;

	// Only register once
	if ( CommandList.IsValid() )
		return;

	TSharedPtr< class FSlateStyleSet > StyleSet = FKeshSweeperStyle::Get();

	if ( !StyleSet.IsValid() )
		return;

	// Assigns style based on the variable name
	UI_COMMAND( OpenWindow, "Kesh Sweeper", "Opens the Kesh Sweeper window", EUserInterfaceActionType::Button, FInputGesture() );

	UTexture2D* TargetIcon = LoadObject< UTexture2D >( nullptr, TEXT( "/Engine/EditorMaterials/TargetIcon" ) );
	check( TargetIcon );
	
	StyleSet->Set( "KeshSweeperStyle.OpenWindow", new FSlateImageBrush( TargetIcon, FVector2D( 40.f, 40.f ) ) );

	// Start a scope
	{
		// Create command list and add it to the editor
		CommandList = MakeShareable( new FUICommandList );

		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );
		CommandList->Append( LevelEditorModule.GetGlobalLevelEditorActions() );

		CommandList->MapAction(
			OpenWindow,
			FExecuteAction::CreateRaw( FKeshSweeperEditorModule::Plugin.Get(), &FKeshSweeperEditorModule::OnToolbarButtonClicked ),
			FCanExecuteAction::CreateRaw( FKeshSweeperEditorModule::Plugin.Get(), &FKeshSweeperEditorModule::CanClickToolbarButton )
		);

		struct Local
		{
			static void AddToolbarCommands( FToolBarBuilder& ToolbarBuilder )
			{
				ToolbarBuilder.AddToolBarButton( FKeshSweeperCommands::Get().OpenWindow );
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
	// Scope
}

#undef LOCTEXT_NAMESPACE
