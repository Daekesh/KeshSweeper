// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#include "KeshSweeperTab.h"
#include "KeshSweeperEditorModule.h"
#include "KeshSweeperGameController.h"
#include "KeshSweeperGameView.h"
#include "KeshSweeperStyle.h"
#include "Styling/SlateStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SCanvas.h"

#define LOCTEXT_NAMESPACE "FKeshSweeperEditorModule"

const FName SKeshSweeperTab::TabName( "Kesh Sweeper" );

// See FKeshSweeperGameController::EGameStatus
const TArray< FText > SKeshSweeperTab::StatusText(
		std::initializer_list< FText >( {
					LOCTEXT( "Game Not Started", "Not Started" ),
					LOCTEXT( "Game In Progress", "In Progress" ),
					LOCTEXT( "Game Lost", "Lost" ),
					LOCTEXT( "Game Won", "Won" ),
					LOCTEXT( "Game Error", "Error" )
			} )
	);

SKeshSweeperTab::SKeshSweeperTab()
{
	View                         = nullptr;
	NewMinefieldWidthSlider      = nullptr;
	NewMinefieldHeightSlider     = nullptr;
	NewMinefieldDifficultySlider = nullptr;
	StartNewGameButton           = nullptr;
	GameWindow                   = nullptr;
	MinefieldContainer           = nullptr;
	Minefield                    = nullptr;
}

void SKeshSweeperTab::Construct( const FArguments& InArgs )
{
	View = InArgs._View;

	if ( !View.IsValid() ) { return; }

	TSharedPtr< class FSlateStyleSet > StyleSet = FKeshSweeperStyle::Get();

	if ( !StyleSet.IsValid() ) { return; }

// @formatter:off

	SDockTab::Construct(
		SDockTab::FArguments()
			.TabRole( NomadTab )
			.OnTabClosed( View.Get(), &FKeshSweeperGameView::OnTabClosed )
			[
				SAssignNew( GameWindow, SBox )
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
												SAssignNew( NewMinefieldWidthSlider, SSpinBox< uint8 > )
													.MinSliderValue( 1 )
													.MaxSliderValue( this, &SKeshSweeperTab::GetMaximumMinefieldWidth )
													.Value( View->GetNewMinefieldWidth() )
													.MinDesiredWidth( 100.f )
													.OnValueChanged( View.Get(), &FKeshSweeperGameView::OnNewMinefieldWidthSliderValueChanged )
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
												SAssignNew(	NewMinefieldHeightSlider, SSpinBox< uint8 > )
													.MinSliderValue( 1 )
													.MaxSliderValue( this, &SKeshSweeperTab::GetMaximumMinefieldHeight	)
													.Value( View->GetNewMinefieldHeight() )
													.MinDesiredWidth( 100.f )
													.OnValueChanged( View.Get(), &FKeshSweeperGameView::OnNewMinefieldHeightSliderValueChanged )
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
												SAssignNew( NewMinefieldDifficultySlider, SSlider )
													.MinValue( 1.f )
													.MaxValue( 10.f )
													.Value( View->GetNewMinefieldDifficulty() )
													.SliderBarColor( this, &SKeshSweeperTab::GetDifficultySliderBarColour )
													.OnValueChanged( View.Get(), &FKeshSweeperGameView::OnNewMinefieldDifficultySliderValueChanged )
											]

										+ SHorizontalBox::Slot()
											.AutoWidth()
											.Padding( FMargin( 5.f, 4.f, 0.f, 0.f ) )
											[
												SNew( STextBlock )
													.Font( StyleSet->GetFontStyle( "KeshSweeperStyle.MenuText" ) )
													.MinDesiredWidth( 50.f )
													.Text( this, &SKeshSweeperTab::GetNewMinefieldMineCount )
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
													.Text( this, &SKeshSweeperTab::GetCurrentGameStatusText )
											]

										+ SHorizontalBox::Slot()
											.AutoWidth()
											[
												SAssignNew( StartNewGameButton, SButton )
													.HAlign( HAlign_Center )
													.VAlign( VAlign_Center )
													.Text( LOCTEXT( "Button", "New Game" ) )
											]
									]
							]

						// Main game area
						+ SVerticalBox::Slot()
							.Padding( 10.f, 10.f, 10.f, 10.f )
							[
								SAssignNew(	MinefieldContainer, SBox )
								[
									SAssignNew( Minefield, SCanvas )
								]
							]
				]
			]
	);

	// @formatter:on
}

TOptional< uint8 > SKeshSweeperTab::GetMaximumMinefieldWidth() const
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

	return static_cast< uint8 >( Width );
}

TOptional< uint8 > SKeshSweeperTab::GetMaximumMinefieldHeight() const
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

	return static_cast< uint8 >( Height );
}

FSlateColor SKeshSweeperTab::GetDifficultySliderBarColour() const
{
	static const FSlateColor DefaultReturn = FSlateColor( FLinearColor::White );

	if ( !NewMinefieldDifficultySlider.IsValid() ) { return DefaultReturn; }

	return FSlateColor( FLinearColor(
			0.25f + ( 0.75f / 9.f * ( NewMinefieldDifficultySlider->GetValue() - 1.f ) ),
			1.f - ( 0.75f / 9.f * ( NewMinefieldDifficultySlider->GetValue() - 1.f ) ),
			0.25f
		) );
}

FText SKeshSweeperTab::GetNewMinefieldMineCount() const
{
	uint16 MineCount = FKeshSweeperGameController::GetMineCountForDifficulty(
			NewMinefieldWidthSlider->GetValue() * NewMinefieldHeightSlider->GetValue(),
			NewMinefieldDifficultySlider->GetValue()
		);

	return FText::FromString( FString::FromInt( MineCount ) );
}

FText SKeshSweeperTab::GetCurrentGameStatusText() const
{
	static const FText ErrorReturn = LOCTEXT( "Error", "Error" );

	FKeshSweeperEditorModule* KeshSweeperModule = FKeshSweeperEditorModule::GetPtr();

	if ( !KeshSweeperModule)
		return ErrorReturn;

	if ( !KeshSweeperModule->GetController().IsValid() )
		return ErrorReturn;

	return StatusText[ KeshSweeperModule->GetController()->GetGameStatus() ];
}

void SKeshSweeperTab::FixWidgets() const
{
	if ( !View.IsValid() )
		return;

	// Slate doesn't do this automatically, even when setting max value and max slider value?!
	if ( NewMinefieldWidthSlider.IsValid() )
	{
		if ( NewMinefieldWidthSlider->GetValue() > NewMinefieldWidthSlider->GetMaxSliderValue() )
		{
			View->SetNewMinefieldWidth( NewMinefieldWidthSlider->GetMaxSliderValue() );
		}
	}

	if ( NewMinefieldHeightSlider.IsValid() )
	{
		if ( NewMinefieldHeightSlider->GetValue() > NewMinefieldHeightSlider->GetMaxSliderValue() )
		{
			View->SetNewMinefieldHeight( NewMinefieldHeightSlider->GetMaxSliderValue() );
		}
	}

	FKeshSweeperEditorModule* KeshSweeperModule = FKeshSweeperEditorModule::GetPtr();

	// For some reason the vertical box won't use the full height? Well, let's force it.
	if ( GameWindow.IsValid() && KeshSweeperModule && KeshSweeperModule->GetModel().IsValid() )
	{
		float TotalWidth   = GameWindow->GetCachedGeometry().GetLocalSize().X - 20.f;
		float WidthPadding = TotalWidth - ( KeshSweeperModule->GetModel()->GetMinefieldWidth() * FKeshSweeperGameView::CellSize );

		if ( WidthPadding < 0.f )
			WidthPadding = 0.f;

		float TotalHeight   = GameWindow->GetCachedGeometry().GetLocalSize().Y - 65.f;
		float HeightPadding = TotalHeight - ( KeshSweeperModule->GetModel()->GetMinefieldHeight() * FKeshSweeperGameView::CellSize );

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
