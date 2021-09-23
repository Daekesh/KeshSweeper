#include "KeshSweeperCommands.h"

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
	// Assigns style based on the variable name (see below) and the style set name (see above)
	UI_COMMAND( ToggleWindow, "Kesh Sweeper", "Toggles the Kesh Sweeper window", EUserInterfaceActionType::Button, FInputGesture() );

	TSharedPtr< class FSlateStyleSet > StyleSet = FKeshSweeperStyle::Get();
	UTexture2D* TargetIcon = LoadObject< UTexture2D >( nullptr, TEXT( "/Engine/EditorMaterials/TargetIcon" ) );
	StyleSet->Set( "KeshSweeperStyle.ToggleWindow", new FSlateImageBrush( TargetIcon, FVector2D( 40.f, 40.f ) ) );
}

#undef LOCTEXT_NAMESPACE
