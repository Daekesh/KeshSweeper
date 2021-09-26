// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#include "KeshSweeperMinefieldCell.h"
#include "KeshSweeperGameModel.h"
#include "KeshSweeperStyle.h"

#define LOCTEXT_NAMESPACE "FKeshSweeperEditorModule"

// Avoid namespace pollution
namespace ECellLayers
{
	enum Enum
	{
		Background,
		ExplodedBackground,
		Mine,
		NearbyMines,
		Button,
		Suspected
	};
}

SKeshSweeperMinefieldCell::SKeshSweeperMinefieldCell()
{
	Model = nullptr;
	Loc = { 0, 0 };
}

void SKeshSweeperMinefieldCell::Construct( const FArguments& InArgs )
{
	Model = InArgs._Model;
	Loc = InArgs._Loc;

	TSharedPtr< class FSlateStyleSet > StyleSet = FKeshSweeperStyle::Get();

	if ( !StyleSet.IsValid() )
		return;

	Background = SNew( SColorBlock )
		.Color( StyleSet->GetColor( "KeshSweeperStyle.Background" ) );

	ExplodedBackground = SNew( SColorBlock )
		.Color( StyleSet->GetColor( "KeshSweeperStyle.ExplodedBackground" ) )
		.Visibility( EVisibility::Hidden );

	Mine = SNew( SBox )
		.Padding( 8.f )
		.MaxDesiredWidth( FKeshSweeperGameView::CellSize - 16.f )
		.MaxDesiredHeight( FKeshSweeperGameView::CellSize - 16.f )
		.Visibility( EVisibility::Hidden )
		[
			SNew( SImage )
			.Image( StyleSet->GetBrush( "KeshSweeperStyle.MineBrush" ) )
		];

	NearbyMines = SNew( STextBlock )
		.Text( LOCTEXT( "0", "0" ) )
		.Font( StyleSet->GetFontStyle( "KeshSweeperStyle.NearbyMineText" ) )
		.ColorAndOpacity( StyleSet->GetColor( "KeshSweeperStyle.NearbyMineTextColour.1" ) )
		.Visibility( EVisibility::Hidden );

	Button = SNew( SImage )
		.Image( StyleSet->GetBrush( "KeshSweeperStyle.ButtonBrush" ) );

	Suspect = SNew( SBox )
		.Padding( FMargin( 8.f, 6.f, 8.f, 10.f ) )
		.MaxDesiredWidth( FKeshSweeperGameView::CellSize - 16.f )
		.MaxDesiredHeight( FKeshSweeperGameView::CellSize - 16.f )
		.Visibility( EVisibility::Hidden )
		[
			SNew( SImage )
			.Image( StyleSet->GetBrush( "KeshSweeperStyle.SuspectBrush" ) )
		];

	AddSlot( ECellLayers::Background         )[ Background         .ToSharedRef() ];	
	AddSlot( ECellLayers::ExplodedBackground )[ ExplodedBackground .ToSharedRef() ];	
	AddSlot( ECellLayers::Mine               )[ Mine               .ToSharedRef() ];	
	AddSlot( ECellLayers::NearbyMines        )
		.HAlign( HAlign_Center )
		.VAlign( VAlign_Center )              [ NearbyMines        .ToSharedRef() ];	
	AddSlot( ECellLayers::Button             )[ Button             .ToSharedRef() ];	
	AddSlot( ECellLayers::Suspected          )[ Suspect            .ToSharedRef() ];
}

void SKeshSweeperMinefieldCell::UpdateDisplay()
{
	if ( !Model.IsValid() )
		return;

	TSharedPtr< class FSlateStyleSet > StyleSet = FKeshSweeperStyle::Get();

	if ( !StyleSet.IsValid() )
		return;

	const FCellInfo& CellInfo = Model->GetCellInfo( Loc );
	
#define VisibilityMacro( Widget, NewVisibility ) \
	if ( Widget.IsValid() && Widget->GetVisibility() != NewVisibility ) Widget->SetVisibility( NewVisibility );

	switch ( CellInfo.Status )
	{
		case ECellStatus::Hidden:
			VisibilityMacro( Background,         EVisibility::Visible );
			VisibilityMacro( ExplodedBackground, EVisibility::Hidden  );
			VisibilityMacro( Mine,               EVisibility::Hidden  );
			VisibilityMacro( NearbyMines,        EVisibility::Hidden  );
			VisibilityMacro( Button,             EVisibility::Visible );
			VisibilityMacro( Suspect,            EVisibility::Hidden  );
			break;

		case ECellStatus::Suspected:
			VisibilityMacro( Background,         EVisibility::Visible );
			VisibilityMacro( ExplodedBackground, EVisibility::Hidden  );
			VisibilityMacro( Mine,               EVisibility::Hidden  );
			VisibilityMacro( NearbyMines,        EVisibility::Hidden  );
			VisibilityMacro( Button,             EVisibility::Visible );
			VisibilityMacro( Suspect,            EVisibility::Visible );
			break;

		case ECellStatus::Revealed:
			VisibilityMacro( Background,         EVisibility::Visible );
			VisibilityMacro( ExplodedBackground, EVisibility::Hidden  );
			VisibilityMacro( Mine,               ( CellInfo.bIsMine ? EVisibility::Visible : EVisibility::Hidden ) );
			VisibilityMacro( NearbyMines,        EVisibility::Hidden  ); // Updated later
			VisibilityMacro( Button,             EVisibility::Hidden  );
			VisibilityMacro( Suspect,            EVisibility::Hidden  );
			break;

		case ECellStatus::Exploded:
			VisibilityMacro( Background,         EVisibility::Hidden  );
			VisibilityMacro( ExplodedBackground, EVisibility::Visible );
			VisibilityMacro( Mine,               EVisibility::Visible );
			VisibilityMacro( NearbyMines,        EVisibility::Hidden  );
			VisibilityMacro( Button,             EVisibility::Hidden  );
			VisibilityMacro( Suspect,            EVisibility::Hidden  );
			break;
	}

	if ( CellInfo.Status == ECellStatus::Revealed && !CellInfo.bIsMine )
	{
		uint16 NearbyMineCount = Model->GetNearbyMineCount( Loc );

		if ( NearbyMineCount > 8 )
			NearbyMineCount = 8;

		if ( NearbyMineCount > 0 )
		{
			NearbyMines->SetText( FText::FromString( FString::FromInt( NearbyMineCount ) ) );
			NearbyMines->SetColorAndOpacity( StyleSet->GetColor( FName( "KeshSweeperStyle.NearbyMineTextColour." + FString::FromInt( NearbyMineCount ) ) ) );
			NearbyMines->SetVisibility( EVisibility::Visible );
		}
	}
}

#undef LOCTEXT_NAMESPACE
