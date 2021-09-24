// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "KeshSweeperGameView.h"
#include "KeshSweeperEditorModule.h"
#include "KeshSweeperGameModel.h"
#include "KeshSweeperStyle.h"

#define LOCTEXT_NAMESPACE "FKeshSweeperEditorModule"

const FName FKeshSweeperGameView::TabName( "Kesh Sweeper" );

// See FKeshSweeperGameController::EGameStatus
const TArray< FText > FKeshSweeperGameView::FKeshSweeperGameView::StatusText( std::initializer_list< FText >( {
	LOCTEXT( "Game Not Started", "Not Started" ),
	LOCTEXT( "Game In Progress", "In Progress" ),
	LOCTEXT( "Game Lost",        "Lost" ),
	LOCTEXT( "Game Won",         "Won" ),
	LOCTEXT( "Game Error",       "Error" )
} ) );

const float FKeshSweeperGameView::CellSize = 32.f;

FKeshSweeperGameView::FKeshSweeperGameView( TSharedPtr< class FKeshSweeperEditorModule > InPlugin )
{
	Plugin = InPlugin;

	// Default slider values
	NewMinefieldWidth = 1;
	NewMinefieldHeight = 1;
	NewMinefieldDifficulty = 1.f;

	// Minefield click detection
	MouseDownButton = EKeys::Invalid;
	MouseDownCell.X = -1;
	MouseDownCell.Y = -1;
}

void FKeshSweeperGameView::Init()
{
	// Styles
	TSharedPtr< class FSlateStyleSet > StyleSet = FKeshSweeperStyle::Get();

	if ( !StyleSet.IsValid() )
		return;

	// Load editor icons. Make sure they are valid.
	static UTexture2D* ButtonIcon = LoadObject< UTexture2D >( nullptr, TEXT( "/Engine/MobileResources/HUD/MobileHUDButton3.MobileHUDButton3" ) );
	check( ButtonIcon );

	static UTexture2D* MineIcon = LoadObject< UTexture2D >( nullptr, TEXT( "/Engine/MobileResources/HUD/AnalogHat.AnalogHat" ) );
	check( MineIcon );

	static UTexture2D* SuspectIcon = LoadObject< UTexture2D >( nullptr, TEXT( "/Engine/EditorResources/Waypoint.Waypoint" ) );
	check( SuspectIcon );

	// Set style values
	StyleSet->Set( "KeshSweeperStyle.Background",             FLinearColor( 0.5f, 0.5f, 0.5f, 0.5f ) ); // Grey
	StyleSet->Set( "KeshSweeperStyle.ExplodedBackground",     FLinearColor( 1.0f, 0.25f, 0.25f ) ); // Red
	StyleSet->Set( "KeshSweeperStyle.MenuText",               FCoreStyle::GetDefaultFontStyle( "Regular", 12 ) );
	StyleSet->Set( "KeshSweeperStyle.NearbyMineText",         FCoreStyle::GetDefaultFontStyle( "Regular", 12 ) );
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.1", FLinearColor( 0.0f, 0.0f, 0.0f ) ); // 1 Black 
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.2", FLinearColor( 0.0f, 0.0f, 1.0f ) ); // 2 Blue 
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.3", FLinearColor( 0.5f, 1.0f, 1.0f ) ); // 3 Cyan
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.4", FLinearColor( 0.2f, 1.0f, 0.0f ) ); // 4 Green
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.5", FLinearColor( 1.0f, 1.0f, 0.0f ) ); // 5 Yellow
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.6", FLinearColor( 1.0f, 0.6f, 0.2f ) ); // 6 Orange
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.7", FLinearColor( 1.0f, 0.0f, 0.0f ) ); // 7 Red
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.8", FLinearColor( 1.0f, 0.8f, 0.8f ) );  // 8 Pinkish?
	
	StyleSet->Set(
		"KeshSweeperStyle.ButtonBrush",
		new FSlateImageBrush(
			ButtonIcon,
			FVector2D( 64.f, 64.f )
		)
	);

	StyleSet->Set(
		"KeshSweeperStyle.MineBrush",
		new FSlateImageBrush(
			MineIcon,
			FVector2D( 32.f, 32.f )
		)
	);

	StyleSet->Set( 
		"KeshSweeperStyle.SuspectBrush", 
		new FSlateImageBrush(
			SuspectIcon,
			FVector2D( 256.f, 256.f ),
			FLinearColor( 1.f, 0.25f, 0.25f )
		)
	);

	// Register the tab spawner so for the button.
	TSharedRef< FGlobalTabmanager > TabManager = FGlobalTabmanager::Get();
	TabManager->RegisterTabSpawner( FKeshSweeperGameView::TabName, FOnSpawnTab::CreateRaw( this, &FKeshSweeperGameView::CreateTab ) );
}

void FKeshSweeperGameView::Destruct()
{
	TSharedRef< FGlobalTabmanager > TabManager = FGlobalTabmanager::Get();
	TabManager->UnregisterTabSpawner( FKeshSweeperGameView::TabName );
}

TSharedRef<SDockTab> FKeshSweeperGameView::CreateTab( const FSpawnTabArgs& TabSpawnArgs )
{
	if ( !Plugin.IsValid() )
		return Tab.ToSharedRef();

	if ( !Plugin->GetModel().IsValid() )
		return Tab.ToSharedRef();

	TSharedPtr< class FSlateStyleSet > StyleSet = FKeshSweeperStyle::Get();

	if ( !StyleSet.IsValid() )
		return Tab.ToSharedRef();

	// Boiler plate macros to clean up the slate code a bit
#define AssignMacro( VarName, InstantiateBody ) \
	( [ &, this ]() { VarName = InstantiateBody; return VarName.ToSharedRef(); } )()

#define GetterMacro( GetType, GetMethod ) \
	TAttribute< GetType >::Create( TAttribute< GetType >::FGetter::CreateSP( this, &FKeshSweeperGameView::GetMethod ) )
	
	// Create the tab layout
	TSharedRef< SDockTab > KeshSweeperTab = SNew( SDockTab )
		.TabRole( ETabRole::NomadTab ) // Dockable anywhere
		.OnTabClosed( this, &FKeshSweeperGameView::OnTabClosed )
		[
			AssignMacro(
				GameWindow,
				SNew( SBox )
					[
						SNew( SVerticalBox )
							// Options bar at the top.
							+ SVerticalBox::Slot()
								.Padding( 10.f, 10.f, 10.f, 10.f )
								.MaxHeight( 25.f )
								.HAlign( HAlign_Fill )
								[
									SNew( SBox )
										.HAlign( HAlign_Center )
										[
												SNew( SHorizontalBox )
													+ SHorizontalBox::Slot()
														.AutoWidth()
														.Padding( FMargin( 0.f, 4.f, 20.f, 0.f ) )
														[
															SNew( STextBlock )
																.Font( StyleSet->GetFontStyle( "KeshSweeperStyle.MenuText" ) )
																.Text( LOCTEXT( "Width", "Width" ) )
														]
													+ SHorizontalBox::Slot()
														.MaxWidth( 100.f )
														[
															AssignMacro(
																NewMinefieldWidthSlider,
																SNew( SSpinBox< uint8 > )
																	.MinSliderValue( 1 )
																	.MaxSliderValue( GetterMacro( TOptional< uint8 >, GetMaximumMinefieldWidth ) )
																	.Value( NewMinefieldWidth )
																	.MinDesiredWidth( 100.f )
																	.OnValueChanged( this, &FKeshSweeperGameView::OnNewMinefieldWidthChanged )
															)
														]
													+ SHorizontalBox::Slot()
														.AutoWidth()
														.Padding( FMargin( 20.f, 4.f, 20.f, 0.f ) )
														[
															SNew( STextBlock )
																.Font( StyleSet->GetFontStyle( "KeshSweeperStyle.MenuText" ) )
																.Text( LOCTEXT( "Height", "Height" ) )
														]
													+ SHorizontalBox::Slot()
														.MaxWidth( 100.f )
														[
															AssignMacro(
																NewMinefieldHeightSlider,
																SNew( SSpinBox< uint8 > )
																	.MinSliderValue( 1 )
																	.MaxSliderValue( GetterMacro( TOptional< uint8 >, GetMaximumMinefieldHeight ) )
																	.Value( NewMinefieldHeight )
																	.MinDesiredWidth( 100.f )
																	.OnValueChanged( this, &FKeshSweeperGameView::OnNewMinefieldHeightChanged )
															)
														]
													+ SHorizontalBox::Slot()
														.AutoWidth()
														.Padding( FMargin( 20.f, 4.f, 20.f, 0.f ) )
														[
															SNew( STextBlock )
																.Font( StyleSet->GetFontStyle( "KeshSweeperStyle.MenuText" ) )
																.Text( LOCTEXT( "Difficulty", "Difficulty" ) )
														]
													+ SHorizontalBox::Slot()
														.MaxWidth( 100.f )
														[
															AssignMacro(
																NewMinefieldDifficultySlider,
																SNew( SSlider )
																	.MinValue( 1.f )
																	.MaxValue( 10.f )
																	.Value( NewMinefieldDifficulty )
																	.SliderBarColor( GetterMacro( FSlateColor, GetDifficultySliderBarColour ) )
																	.OnValueChanged( this, &FKeshSweeperGameView::OnNewMinefieldDifficultyChanged )
															)
														]
													+ SHorizontalBox::Slot()
														.AutoWidth()
														.Padding( FMargin( 5.f, 4.f, 0.f, 0.f ) )
														[
															SNew( STextBlock )
																.Font( StyleSet->GetFontStyle( "KeshSweeperStyle.MenuText" ) )
																.MinDesiredWidth( 50.f )
																.Text( GetterMacro( FText, GetNewMinefieldMineCount ) )
														]
													+ SHorizontalBox::Slot()
														.AutoWidth()
														.HAlign( HAlign_Right )
														.Padding( FMargin( 20.f, 4.f, 20.f, 0.f ) )
														[
															SNew( STextBlock )
																.Font( StyleSet->GetFontStyle( "KeshSweeperStyle.MenuText" ) )
																.Text( LOCTEXT( "Status:", "Status:" ) )
														]
													+ SHorizontalBox::Slot()
														.AutoWidth()
														.Padding( FMargin( 0.f, 4.f, 50.f, 0.f ) )
														[
															SNew( STextBlock )
																.Font( StyleSet->GetFontStyle( "KeshSweeperStyle.MenuText" ) )
																.Text( GetterMacro( FText, GetCurrentGameStatusText ) )
														]
													+ SHorizontalBox::Slot()
														.AutoWidth()
														[
															SNew( SButton )
																.HAlign( HAlign_Center )
																.VAlign( VAlign_Center )
																.Text( LOCTEXT( "Button", "New Game" ) )
																.OnClicked( this, &FKeshSweeperGameView::OnNewGameButtonClicked )
														]
										]
								]
							// Main game area
							+ SVerticalBox::Slot()
								.Padding( 10.f, 10.f, 10.f, 10.f )
								[
									AssignMacro( 
										MinefieldContainer,
										SNew( SBox )
											[
												AssignMacro(
													Minefield,
													SNew( SCanvas )
												)
											]
									)
								]
					]
			)
		];


	// Hook in the "button" process
	Minefield->SetOnMouseButtonDown( FPointerEventHandler::CreateSP( this, &FKeshSweeperGameView::OnMinefieldMouseButtonDown ) );
	Minefield->SetOnMouseButtonUp( FPointerEventHandler::CreateSP( this, &FKeshSweeperGameView::OnMinefieldMouseButtonUp ) );

	PopulateMinefield();

	return KeshSweeperTab;
}

void FKeshSweeperGameView::ShowTab()
{
	TSharedRef< FGlobalTabmanager > TabManager = FGlobalTabmanager::Get();
	Tab = TabManager->TryInvokeTab( FKeshSweeperGameView::TabName );
	
	if ( !Tab.IsValid() )
		return;

	InvalidateMinefield();
}

void FKeshSweeperGameView::OnTabClosed( TSharedRef< SDockTab > TabClosed )
{
	// Make sure the tab doesn't hang around because we're referencing it!
	Tab.Reset();
}

void FKeshSweeperGameView::SetNewMinefieldWidth( uint8 NewVal )
{
	OnNewMinefieldWidthChanged( NewVal );

	if ( NewMinefieldWidthSlider.IsValid() )
		NewMinefieldWidthSlider->SetValue( NewVal );
}

void FKeshSweeperGameView::SetNewMinefieldHeight( uint8 NewVal )
{
	OnNewMinefieldHeightChanged( NewVal );

	if ( NewMinefieldHeightSlider.IsValid() )
		NewMinefieldHeightSlider->SetValue( NewVal );
}

void FKeshSweeperGameView::SetNewMinefieldDifficulty( float NewVal )
{
	OnNewMinefieldDifficultyChanged( NewVal );

	if ( NewMinefieldDifficultySlider.IsValid() )
		NewMinefieldDifficultySlider->SetValue( NewVal );
}

void FKeshSweeperGameView::OnNewMinefieldWidthChanged( uint8 NewVal )
{
	NewMinefieldWidth = NewVal;
}

void FKeshSweeperGameView::OnNewMinefieldHeightChanged( uint8 NewVal )
{
	NewMinefieldHeight = NewVal;
}

void FKeshSweeperGameView::OnNewMinefieldDifficultyChanged( float NewVal )
{
	NewMinefieldDifficulty = NewVal;
}

TOptional< uint8 > FKeshSweeperGameView::GetMaximumMinefieldWidth() const
{
	if ( !GameWindow.IsValid() )
		return 255;

	float Width = GameWindow->GetCachedGeometry().GetLocalSize().X - 20.f;

	// Either it's really small or there's no cached geometry
	if ( Width < FKeshSweeperGameView::CellSize )
		return 255;

	Width /= FKeshSweeperGameView::CellSize;

	if ( Width < 1.f )
		Width = 1.f;

	else if ( Width > 255.f )
		Width = 255.f;
	
	return ( uint8 ) Width;
}

TOptional< uint8 > FKeshSweeperGameView::GetMaximumMinefieldHeight() const
{
	if ( !GameWindow.IsValid() )
		return 255;

	float Height = GameWindow->GetCachedGeometry().GetLocalSize().Y - 65.f;

	// Either it's really small or there's no cached geometry
	if ( Height < FKeshSweeperGameView::CellSize )
		return 255;

	Height /= FKeshSweeperGameView::CellSize;

	if ( Height < 1.f )
		Height = 1.f;

	else if ( Height > 255.f )
		Height = 255.f;

	return ( uint8 ) Height;
}

FSlateColor FKeshSweeperGameView::GetDifficultySliderBarColour() const
{
	static const FSlateColor DefaultReturn = FSlateColor( FLinearColor::White );

	if ( !NewMinefieldDifficultySlider.IsValid() )
		return DefaultReturn;

	return FSlateColor( FLinearColor(
		0.25f + ( 0.75f / 9.f * ( NewMinefieldDifficultySlider->GetValue() - 1.f ) ),
		1.f - ( 0.75f / 9.f * ( NewMinefieldDifficultySlider->GetValue() - 1.f ) ),
		0.25f
	) );
}

FText FKeshSweeperGameView::GetNewMinefieldMineCount() const
{
	uint16 MineCount = FKeshSweeperGameController::GetMineCountForDifficulty( 
		NewMinefieldWidth * NewMinefieldHeight,
		NewMinefieldDifficulty 
	);	

	return FText::FromString( FString::FromInt( MineCount ) );
}

FText FKeshSweeperGameView::GetCurrentGameStatusText() const
{
	static const FText ErrorReturn = LOCTEXT( "Error", "Error" );

	if ( !Plugin.IsValid() )
		return ErrorReturn;

	if ( !Plugin->GetController().IsValid() )
		return ErrorReturn;

	return FKeshSweeperGameView::StatusText[ Plugin->GetController()->GetGameStatus() ];
}

FReply FKeshSweeperGameView::OnNewGameButtonClicked()
{
	if ( !Plugin.IsValid() )
		return FReply::Handled();

	if ( !Plugin->GetController().IsValid() )
		return FReply::Handled();

	Plugin->GetController()->StartNewGame( NewMinefieldWidth, NewMinefieldHeight, NewMinefieldDifficulty );
	
	return FReply::Handled();
}

bool FKeshSweeperGameView::PopulateMinefield()
{
	if ( !Plugin.IsValid() )
		return false;

	if ( !Plugin->GetModel().IsValid() )
		return false;
	
	if ( !MinefieldContainer.IsValid() )
		return false;

	if ( !Minefield.IsValid() )
		return false;

	MinefieldContainer->SetWidthOverride( Plugin->GetModel()->GetMinefieldWidth() * FKeshSweeperGameView::CellSize );
	MinefieldContainer->SetHeightOverride( Plugin->GetModel()->GetMinefieldHeight() * FKeshSweeperGameView::CellSize );

	Minefield->ClearChildren();
	
	for ( uint8 HeightIndex = 0; HeightIndex < Plugin->GetModel()->GetMinefieldHeight(); ++HeightIndex )
	{
		for ( uint8 WidthIndex = 0; WidthIndex < Plugin->GetModel()->GetMinefieldWidth(); ++WidthIndex )
		{
			Minefield->AddSlot()
				.Position( FVector2D( WidthIndex * FKeshSweeperGameView::CellSize, HeightIndex * FKeshSweeperGameView::CellSize ) )
				.Size( FVector2D( FKeshSweeperGameView::CellSize, FKeshSweeperGameView::CellSize ) )
				[
					SNew( SKeshSweeperMinefieldCell )
						.Plugin( Plugin )
						.Loc( { WidthIndex, HeightIndex } )
				];
		}
	}

	// Update the visuals of every cell
	InvalidateMinefield();

	return true;
}

FCellLocation FKeshSweeperGameView::MouseEventToCellLocation( const FGeometry& Geometry, const FPointerEvent& Event )
{
	FVector2D MouseLocationOnWidget = Event.GetScreenSpacePosition() - Geometry.AbsolutePosition;
	
	// Unscale position to get it back to "CellSize"
	MouseLocationOnWidget /= Geometry.Scale; 

	uint8 CellX = MouseLocationOnWidget.X / FKeshSweeperGameView::CellSize;
	uint8 CellY = MouseLocationOnWidget.Y / FKeshSweeperGameView::CellSize;
	
	return { CellX, CellY };
}

FReply FKeshSweeperGameView::OnMinefieldMouseButtonDown( const FGeometry& Geometry, const FPointerEvent& Event )
{
	// If we're being funny with mouse buttons, abort the click process.
	if ( Event.GetEffectingButton() != EKeys::LeftMouseButton && 
		 Event.GetEffectingButton() != EKeys::RightMouseButton )
	{
		MouseDownButton = EKeys::Invalid;
	}

	// If we've already started the click process, abort it.
	else if ( MouseDownButton != EKeys::Invalid )
		MouseDownButton = EKeys::Invalid;

	// Start a new click process
	else
	{
		MouseDownButton = Event.GetEffectingButton();
		MouseDownCell = FKeshSweeperGameView::MouseEventToCellLocation( Geometry, Event );
	}

	return FReply::Handled();
}

FReply FKeshSweeperGameView::OnMinefieldMouseButtonUp( const FGeometry& Geometry, const FPointerEvent& Event )
{
	// If we're being funny with mouse buttons, abort the click process.
	if ( Event.GetEffectingButton() != EKeys::LeftMouseButton &&
		 Event.GetEffectingButton() != EKeys::RightMouseButton )
	{
		;
	}

	// Started the click process with a different button, abort it.
	else if ( MouseDownButton != Event.GetEffectingButton() )
		;

	// Continue the click process
	else
	{
		FCellLocation MouseUpCell = FKeshSweeperGameView::MouseEventToCellLocation( Geometry, Event );

		// If we've "clicked" the cell
		if ( MouseUpCell.X == MouseDownCell.X && MouseUpCell.Y == MouseDownCell.Y )
			OnMinefieldClicked( Event.GetEffectingButton(), MouseUpCell );

	}

	// Make sure to end the click process
	MouseDownButton = EKeys::Invalid;

	return FReply::Handled();
}

void FKeshSweeperGameView::OnMinefieldClicked( FKey MouseButton, const FCellLocation& Cell )
{
	if ( !Plugin.IsValid() )
		return;

	if ( !Plugin->GetController().IsValid() )
		return;
	
	if ( MouseButton == EKeys::LeftMouseButton )
		Plugin->GetController()->RevealCell( Cell );

	else if ( MouseButton == EKeys::RightMouseButton )
		Plugin->GetController()->SuspectCell( Cell );
}

// Helper function
template< class WidgetClass >
WidgetClass* ConvertWidget( TSharedPtr< const SWidget > WidgetPtr )
{
	// Cast to pointer
	const WidgetClass* ConstWidget = ( const WidgetClass* ) WidgetPtr.Get();

	// Remove the const (we're not changing anything that would affect this)
	WidgetClass* MutableWidget = const_cast< WidgetClass* >( ConstWidget );

	return MutableWidget;
}

void FKeshSweeperGameView::InvalidateMinefield()
{
	if ( !Plugin.IsValid() )
		return;

	if ( !Plugin->GetModel().IsValid() )
		return;

	if ( !Minefield.IsValid() )
		return;

	// Get the minefield cells
	FChildren* Cells = Minefield->GetChildren();

	// Iterate over them
	for ( int Index = 0; Index < Cells->Num(); ++Index )
	{
		// Get the child at the given index
		SKeshSweeperMinefieldCell* Child = ConvertWidget< SKeshSweeperMinefieldCell >( Cells->GetChildAt( Index ) );

		// Update the cell graphic
		Child->Invalidate();
	}
}

void FKeshSweeperGameView::InvalidateCell( const FCellLocation& Loc )
{
	if ( !Plugin.IsValid() )
		return;

	if ( !Plugin->GetModel().IsValid() )
		return;

	if ( !Minefield.IsValid() )
		return;

	if ( Loc.X > Plugin->GetModel()->GetMinefieldWidth() )
		return;

	if ( Loc.Y > Plugin->GetModel()->GetMinefieldHeight() )
		return;

	int Index = Plugin->GetModel()->XYToIndex( Loc );

	// Get the grid cells
	FChildren* Cells = Minefield->GetChildren();

	// Get the child at the given index
	SKeshSweeperMinefieldCell* Child = ConvertWidget< SKeshSweeperMinefieldCell >( Cells->GetChildAt( Index ) );

	// Update the cell graphic
	Child->Invalidate();
}

void FKeshSweeperGameView::TickUI()
{
	if ( !IsTabOpen() )
		return;

	// Slate doesn't do this automatically, even when setting max value and max slider value?!
	if ( NewMinefieldWidthSlider.IsValid() )
	{
		if ( NewMinefieldWidthSlider->GetValue() > NewMinefieldWidthSlider->GetMaxSliderValue() )
			SetNewMinefieldWidth( NewMinefieldWidthSlider->GetMaxSliderValue() );
	}

	if ( NewMinefieldHeightSlider.IsValid() )
	{
		if ( NewMinefieldHeightSlider->GetValue() > NewMinefieldHeightSlider->GetMaxSliderValue() )
			SetNewMinefieldHeight( NewMinefieldHeightSlider->GetMaxSliderValue() );
	}

	// For some reason the vertical box won't use the full height? Well, let's force it.
	if ( GameWindow.IsValid() && Plugin.IsValid() && Plugin->GetModel().IsValid() )
	{
		float TotalWidth = GameWindow->GetCachedGeometry().GetLocalSize().X - 20.f;
		float WidthPadding = TotalWidth - ( Plugin->GetModel()->GetMinefieldWidth() * FKeshSweeperGameView::CellSize );

		if ( WidthPadding < 0.f )
			WidthPadding = 0.f;

		float TotalHeight = GameWindow->GetCachedGeometry().GetLocalSize().Y - 65.f;
		float HeightPadding = TotalHeight - ( Plugin->GetModel()->GetMinefieldHeight() * FKeshSweeperGameView::CellSize );

		if ( HeightPadding < 0.f )
			HeightPadding = 0.f;

		MinefieldContainer->SetPadding( FMargin(
			WidthPadding / 2.f,
			HeightPadding / 2.f,
			WidthPadding / 2.f,
			HeightPadding / 2.f
		) );
	}
}

#undef LOCTEXT_NAMESPACE
