#pragma once

#include "KeshSweeperGameView.h"
#include "KeshSweeperEditorModule.h"
#include "KeshSweeperGameModel.h"
#include "KeshSweeperStyle.h"

#define LOCTEXT_NAMESPACE "FKeshSweeperEditorModule"

const FName FKeshSweeperGameView::TabName( "Kesh Sweeper" );

const TArray<FText> FKeshSweeperGameView::StatusText( std::initializer_list< FText >( {
	LOCTEXT( "Game Not Started", "Not Started" ),
	LOCTEXT( "Game In Progress", "In Progress" ),
	LOCTEXT( "Game Lost",        "Lost" ),
	LOCTEXT( "Game Won",         "Won" ),
	LOCTEXT( "Game Error",       "Error" )
} ) );

const float FKeshSweeperGameView::CellSize = 32.f;

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

FKeshSweeperGameView::FKeshSweeperGameView( TSharedPtr< class FKeshSweeperEditorModule > InPlugin )
{
	// Default slider values
	WidthSliderValue = 1;
	HeightSliderValue = 1;
	DifficultySliderValue = 1.f;

	Plugin = InPlugin;
	MouseDownButton = EKeys::Invalid;
	MouseDownCell.X = -1;
	MouseDownCell.Y = -1;
}

void FKeshSweeperGameView::Init()
{
	// Register the tab spawner so for the button.
	TSharedRef< FGlobalTabmanager > TabManager = FGlobalTabmanager::Get();
	TabManager->RegisterTabSpawner( TabName, FOnSpawnTab::CreateRaw( this, &FKeshSweeperGameView::CreateTab ) );

	// Styles
	TSharedPtr< class FSlateStyleSet > StyleSet = FKeshSweeperStyle::Get();

	if ( !StyleSet.IsValid() )
		return;

	static UTexture2D* ButtonIcon = LoadObject< UTexture2D >( nullptr, TEXT( "/Engine/MobileResources/HUD/MobileHUDButton3.MobileHUDButton3" ) );

	if ( ButtonIcon == nullptr )
		return;

	static UTexture2D* MineIcon = LoadObject< UTexture2D >( nullptr, TEXT( "/Engine/MobileResources/HUD/AnalogHat.AnalogHat" ) );

	if ( MineIcon == nullptr )
		return;

	static UTexture2D* SuspectIcon = LoadObject< UTexture2D >( nullptr, TEXT( "/Engine/EditorResources/Waypoint.Waypoint" ) );

	if ( SuspectIcon == nullptr )
		return;

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

#define GetterMacroClass( GetType, GetMethod, GetObject, GetClass ) \
	TAttribute< GetType >::Create( TAttribute< GetType >::FGetter::CreateSP( GetObject, &GetClass::GetMethod ) )

#define GetterMacro( GetType, GetMethod ) GetterMacroClass( GetType, GetMethod, this, FKeshSweeperGameView )
	
	// Create the tab layout
	TSharedRef<SDockTab> KeshSweeperTab = SNew( SDockTab )
		.TabRole( ETabRole::NomadTab )
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
																WidthInput,
																SNew( SSpinBox< uint8 > )
																	.MinSliderValue( 1 )
																	.MaxSliderValue( GetterMacro( TOptional< uint8 >, GetMaximumWidth ) )
																	.Value( WidthSliderValue ) // Don't use an auto-getter here because it's not based on the current game
																	.OnValueChanged( this, &FKeshSweeperGameView::OnWidthValueChanged )
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
																HeightInput,
																SNew( SSpinBox< uint8 > )
																	.MinSliderValue( 1 )
																	.MaxSliderValue( GetterMacro( TOptional< uint8 >, GetMaximumHeight ) )
																	.Value( HeightSliderValue ) // Don't use an auto-getter here because it's not based on the current game
																	.MinDesiredWidth( 100.f )
																	.OnValueChanged( this, &FKeshSweeperGameView::OnHeightValueChanged )
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
																DifficultyInput,
																SNew( SSlider )
																	.MinValue( 1.f )
																	.MaxValue( 10.f )
																	.Value( DifficultySliderValue )
																	.SliderBarColor( GetterMacro( FSlateColor, GetSliderBarColor ) )
																	.OnValueChanged( this, &FKeshSweeperGameView::OnDifficultyValueChanged )
															)
														]
													+ SHorizontalBox::Slot()
														.AutoWidth()
														.Padding( FMargin( 5.f, 4.f, 0.f, 0.f ) )
														[
															SNew( STextBlock )
																.Font( StyleSet->GetFontStyle( "KeshSweeperStyle.MenuText" ) )
																.MinDesiredWidth( 50.f )
																.Text( GetterMacro( FText, GetMineCount ) )
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
																.Text( GetterMacro( FText, GetStatusText ) )
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
										GridContainer,
										SNew( SBox )
											[
												AssignMacro(
													CellGrid,
													SNew( SCanvas )
												)
											]
									)
								]
					]
			)
		];


	// Hook in the "button" process
	CellGrid->SetOnMouseButtonDown( FPointerEventHandler::CreateSP( this, &FKeshSweeperGameView::OnMouseDown ) );
	CellGrid->SetOnMouseButtonUp( FPointerEventHandler::CreateSP( this, &FKeshSweeperGameView::OnMouseUp ) );

	PopulateGrid( 
		Plugin->GetModel()->GetFieldWidth(), 
		Plugin->GetModel()->GetFieldHeight() 
	);

	return KeshSweeperTab;
}

void FKeshSweeperGameView::ShowTab()
{
	TSharedRef< FGlobalTabmanager > TabManager = FGlobalTabmanager::Get();
	Tab = TabManager->TryInvokeTab( TabName );
	
	if ( Tab.IsValid() )
	{
		Tab->SetVisibility( EVisibility::Visible );
		Invalidate();
	}
}

void FKeshSweeperGameView::TickUI()
{
	if ( !IsTabOpen() )
		return;

	// Slate doesn't do this automatically, even when setting max value and max slider value?!
	if ( WidthInput.IsValid() )
	{
		if ( WidthInput->GetValue() > WidthInput->GetMaxSliderValue() )
			WidthInput->SetValue( WidthInput->GetMaxSliderValue() );
	}

	if ( HeightInput.IsValid() )
	{
		if ( HeightInput->GetValue() > HeightInput->GetMaxSliderValue() )
			HeightInput->SetValue( HeightInput->GetMaxSliderValue() );
	}

	// For some reason the vertical box won't use the full height? Well, let's force it.
	if ( GameWindow.IsValid() && Plugin.IsValid() && Plugin->GetModel().IsValid() )
	{
		float TotalWidth = GameWindow->GetCachedGeometry().GetLocalSize().X - 20.f;
		float WidthPad = TotalWidth - ( Plugin->GetModel()->GetFieldWidth() * CellSize );

		if ( WidthPad < 0.f )
			WidthPad = 0.f;

		float TotalHeight = GameWindow->GetCachedGeometry().GetLocalSize().Y - 70.f;
		float HeightPad = TotalHeight - ( Plugin->GetModel()->GetFieldHeight() * CellSize );

		if ( HeightPad < 0.f )
			HeightPad = 0.f;

		GridContainer->SetPadding( FMargin(
			WidthPad / 2.f,
			HeightPad / 2.f,
			WidthPad / 2.f,
			HeightPad / 2.f
		) );
	}
}

void FKeshSweeperGameView::SetWidthSliderValue( uint8 NewVal )
{
	WidthSliderValue = NewVal;

	if ( WidthInput.IsValid() )
		WidthInput->SetValue( NewVal );
}

void FKeshSweeperGameView::SetHeightSliderValue( uint8 NewVal )
{
	HeightSliderValue = NewVal;

	if ( HeightInput.IsValid() )
		HeightInput->SetValue( NewVal );
}

void FKeshSweeperGameView::SetDifficultySliderValue( float NewVal )
{
	DifficultySliderValue = NewVal;

	if ( DifficultyInput.IsValid() )
		DifficultyInput->SetValue( NewVal );
}

void FKeshSweeperGameView::OnWidthValueChanged( uint8 NewVal )
{
	WidthSliderValue = NewVal;
}

void FKeshSweeperGameView::OnHeightValueChanged( uint8 NewVal )
{
	HeightSliderValue = NewVal;
}

void FKeshSweeperGameView::OnDifficultyValueChanged( float NewVal )
{
	DifficultySliderValue = NewVal;
}

// Helper function
template< class WidgetClass >
WidgetClass* ConvertWidget( TSharedPtr< const SWidget > Widget )
{
	// Cast to pointer
	const WidgetClass* ConstWidget = ( const WidgetClass* ) Widget.Get();

	// Remove the const (we're not changing anything that would affect this)
	WidgetClass* MutableWidget = const_cast< WidgetClass* >( ConstWidget );

	return MutableWidget;
}

void FKeshSweeperGameView::Invalidate()
{
	if ( !Plugin.IsValid() )
		return;

	if ( !Plugin->GetModel().IsValid() )
		return;

	if ( !CellGrid.IsValid() )
		return;

	// Get the grid cells
	FChildren* CellChildren = CellGrid->GetChildren();

	// Iterate over them
	for ( int Index = 0; Index < CellChildren->Num(); ++Index )
	{
		// Get the child at the given index
		SOverlay* Child = ConvertWidget< SOverlay >( CellChildren->GetChildAt( Index ) );

		// Pass it to the updater
		UpdateCellVisual( Plugin->GetModel()->IndexToXY( Index ), Child );
	}
}

void FKeshSweeperGameView::InvalidateCell( const FCellLocation& Loc )
{
	if ( !Plugin.IsValid() )
		return;

	if ( !Plugin->GetModel().IsValid() )
		return;

	if ( !CellGrid.IsValid() )
		return;

	if ( Loc.X > Plugin->GetModel()->GetFieldWidth() )
		return;

	if ( Loc.Y > Plugin->GetModel()->GetFieldHeight() )
		return;

	int Index = Plugin->GetModel()->XYToIndex( Loc );

	// Get the grid cells
	FChildren* CellChildren = CellGrid->GetChildren();

	// Get the child at the given index
	SOverlay* Child = ConvertWidget< SOverlay >( CellChildren->GetChildAt( Index ) );

	// Pass it to the updater
	UpdateCellVisual( Loc, Child );
}

void FKeshSweeperGameView::UpdateCellVisual( const FCellLocation& Loc, SOverlay* Cell )
{
	if ( !Plugin.IsValid() )
		return;

	if ( !Plugin->GetModel().IsValid() )
		return;

	if ( Loc.X > Plugin->GetModel()->GetFieldWidth() )
		return;

	if ( Loc.Y > Plugin->GetModel()->GetFieldHeight() )
		return;

	TSharedPtr< class FSlateStyleSet > StyleSet = FKeshSweeperStyle::Get();

	if ( !StyleSet.IsValid() )
		return;

	const FCellInfo& CellInfo = Plugin->GetModel()->GetCellInfo( Loc );
	FChildren* Children = Cell->GetChildren();

	SColorBlock* Background         = ConvertWidget< SColorBlock >( Children->GetChildAt( ECellLayers::Background ) );
	SColorBlock* ExplodedBackground = ConvertWidget< SColorBlock >( Children->GetChildAt( ECellLayers::ExplodedBackground ) );
	SBox*        Mine               = ConvertWidget< SBox >       ( Children->GetChildAt( ECellLayers::Mine ) );
	STextBlock*  NearbyMines        = ConvertWidget< STextBlock > ( Children->GetChildAt( ECellLayers::NearbyMines ) );
	SImage*      Button             = ConvertWidget< SImage >     ( Children->GetChildAt( ECellLayers::Button ) );
	SBox*        Suspect            = ConvertWidget< SBox >       ( Children->GetChildAt( ECellLayers::Suspected ) );

#define VisibilityMacro( Widget, NewVisibility ) \
	if ( Widget->GetVisibility() != NewVisibility ) Widget->SetVisibility( NewVisibility );

	switch ( CellInfo.Status )
	{
		case ECellStatus::Hidden:
			VisibilityMacro( Background,         EVisibility::Visible );
			VisibilityMacro( ExplodedBackground, EVisibility::Hidden );
			VisibilityMacro( Mine,               EVisibility::Hidden );
			VisibilityMacro( NearbyMines,        EVisibility::Hidden );
			VisibilityMacro( Button,             EVisibility::Visible );
			VisibilityMacro( Suspect,            EVisibility::Hidden );
			break;

		case ECellStatus::Suspect:
			VisibilityMacro( Background,         EVisibility::Visible );
			VisibilityMacro( ExplodedBackground, EVisibility::Hidden );
			VisibilityMacro( Mine,               EVisibility::Hidden );
			VisibilityMacro( NearbyMines,        EVisibility::Hidden );
			VisibilityMacro( Button,             EVisibility::Visible );
			VisibilityMacro( Suspect,            EVisibility::Visible );
			break;

		case ECellStatus::Revealed:
			VisibilityMacro( Background,         EVisibility::Visible );
			VisibilityMacro( ExplodedBackground, EVisibility::Hidden );
			VisibilityMacro( Mine,               ( CellInfo.bIsMine ? EVisibility::Visible : EVisibility::Hidden ) );
			VisibilityMacro( NearbyMines,        ( CellInfo.bIsMine ? EVisibility::Hidden : EVisibility::Visible ) );
			VisibilityMacro( Button,             EVisibility::Hidden );
			VisibilityMacro( Suspect,            EVisibility::Hidden );
			break;

		case ECellStatus::Exploded:
			VisibilityMacro( Background,         EVisibility::Hidden );
			VisibilityMacro( ExplodedBackground, EVisibility::Visible );
			VisibilityMacro( Mine,               EVisibility::Visible );
			VisibilityMacro( NearbyMines,        EVisibility::Hidden );
			VisibilityMacro( Button,             EVisibility::Hidden );
			VisibilityMacro( Suspect,            EVisibility::Hidden );
			break;
	}

	if ( CellInfo.Status == ECellStatus::Revealed && !CellInfo.bIsMine )
	{
		uint16 NearbyMineCount = Plugin->GetModel()->GetNearbyMineCount( Loc );

		if ( NearbyMineCount > 8 )
			NearbyMineCount = 8;

		if ( NearbyMineCount > 0 )
		{
			NearbyMines->SetText( FText::FromString( FString::FromInt( NearbyMineCount ) ) );
			NearbyMines->SetColorAndOpacity( StyleSet->GetColor( FName( "KeshSweeperStyle.NearbyMineTextColour." + FString::FromInt( NearbyMineCount ) ) ) );
		}

		else
			NearbyMines->SetVisibility( EVisibility::Hidden );
	}
}

TOptional< uint8 > FKeshSweeperGameView::GetMaximumWidth() const
{
	if ( !GameWindow.IsValid() )
		return 255;

	float Width = GameWindow->GetCachedGeometry().GetLocalSize().X - 20.f;

	if ( Width < CellSize )
		return 255;

	Width /= CellSize;

	if ( Width < 1.f )
		Width = 1.f;

	else if ( Width > 255.f )
		Width = 255.f;
	
	return ( uint8 ) Width;
}

TOptional< uint8 > FKeshSweeperGameView::GetMaximumHeight() const
{
	if ( !GameWindow.IsValid() )
		return 255;

	float Height = GameWindow->GetCachedGeometry().GetLocalSize().Y - 50.f;

	if ( Height < CellSize )
		return 255;

	Height /= CellSize;

	if ( Height < 1.f )
		Height = 1.f;

	else if ( Height > 255.f )
		Height = 255.f;

	return ( uint8 ) Height;
}

FSlateColor FKeshSweeperGameView::GetSliderBarColor() const
{
	if ( !DifficultyInput.IsValid() )
		return FSlateColor( FLinearColor::White );

	return FSlateColor( FLinearColor(
		0.25f + 0.75f / 9.f * ( DifficultyInput->GetValue() - 1.f ),
		1.f - 0.75f / 9.f * ( DifficultyInput->GetValue() - 1.f ),
		0.25f
	) );
}

FText FKeshSweeperGameView::GetMineCount() const
{
	if ( !Plugin.IsValid() )
		return LOCTEXT( "Error", "Error" );

	if ( !Plugin->GetController().IsValid() )
		return LOCTEXT( "Error", "Error" );

	uint16 MineCount = Plugin->GetController()->GetMineCountForDifficulty( DifficultyInput->GetValue() );	
	FString MineString = FString::FromInt( MineCount );

	return FText::FromString( MineString );
}

FText FKeshSweeperGameView::GetStatusText() const
{
	if ( !Plugin.IsValid() )
		return LOCTEXT( "Error", "Error" );

	if ( !Plugin->GetController().IsValid() )
		return LOCTEXT( "Error", "Error" );

	return FKeshSweeperGameView::StatusText[ Plugin->GetController()->GetGameStatus() ];
}

FReply FKeshSweeperGameView::OnNewGameButtonClicked()
{
	if ( !Plugin.IsValid() )
		return FReply::Handled();

	if ( !Plugin->GetController().IsValid() )
		return FReply::Handled();

	if ( !WidthInput.IsValid() )
		return FReply::Handled();

	if ( !HeightInput.IsValid() )
		return FReply::Handled();

	if ( !DifficultyInput.IsValid() )
		return FReply::Handled();

	Plugin->GetController()->StartNewGame( ( uint8 ) WidthInput->GetValue(), ( uint8 ) HeightInput->GetValue(), DifficultyInput->GetValue() );
	
	return FReply::Handled();
}

bool FKeshSweeperGameView::PopulateGrid( uint8 Width, uint8 Height )
{
	if ( !GridContainer.IsValid() )
		return false;

	if ( !CellGrid.IsValid() )
		return false;

	TSharedPtr< class FSlateStyleSet > StyleSet = FKeshSweeperStyle::Get();

	if ( !StyleSet.IsValid() )
		return false;

	CellGrid->ClearChildren();
	
	for ( uint8 h = 0; h < Height; ++h )
	{
		for ( uint8 w = 0; w < Width; ++w )
		{
			CellGrid->AddSlot()
				.Position( FVector2D( w * CellSize, h * CellSize ) )
				.Size( FVector2D( CellSize, CellSize ) )
				[
					SNew( SOverlay )
						+ SOverlay::Slot()
							[
								// Root background
								SNew( SColorBlock )
									.Color( StyleSet->GetColor( "KeshSweeperStyle.Background" ) )
							]
						+ SOverlay::Slot()
							[
								// Exploded mine background!
								SNew( SColorBlock )
									.Color( StyleSet->GetColor( "KeshSweeperStyle.ExplodedBackground" ) )
									.Visibility( EVisibility::Hidden )
							]
						+ SOverlay::Slot()
							[
								SNew( SBox )
									.Padding( 8.f )
									.MaxDesiredWidth( CellSize - 16.f )
									.MaxDesiredHeight( CellSize - 16.f )
									.Visibility( EVisibility::Hidden )
									[
										// Mine
										SNew( SImage )
											.Image( StyleSet->GetBrush( "KeshSweeperStyle.MineBrush" ) )
									]
							]
						+ SOverlay::Slot()
							.HAlign( HAlign_Center )
							.VAlign( VAlign_Center )
							[
								// Nearby mines
								SNew( STextBlock )
									.Text( LOCTEXT( "0", "0" ) )
									.Font( StyleSet->GetFontStyle( "KeshSweeperStyle.NearbyMineText" ) )
									.ColorAndOpacity( StyleSet->GetColor( "KeshSweeperStyle.NearbyMineTextColour.1" ) )
									.Visibility( EVisibility::Hidden )
							]								
						+SOverlay::Slot()
							[
								// Button graphic
								SNew( SImage )
									.Image( StyleSet->GetBrush( "KeshSweeperStyle.ButtonBrush" ) )
							]
						+ SOverlay::Slot()
							[
								SNew( SBox )
									.Padding( 8.f )
									.MaxDesiredWidth( CellSize - 16.f )
									.MaxDesiredHeight( CellSize - 16.f )
									.Visibility( EVisibility::Hidden )
									[
										// Suspect mark
										SNew( SImage )
											.Image( StyleSet->GetBrush( "KeshSweeperStyle.SuspectBrush" ) )
									]
							]
				];
		}
	}

	GridContainer->SetWidthOverride( Width * CellSize );
	GridContainer->SetHeightOverride( Height * CellSize );	

	Invalidate();

	return true;
}

FCellLocation FKeshSweeperGameView::MouseEventToCellLocation( const FGeometry& Geometry, const FPointerEvent& Event )
{
	FVector2D MouseLocationOnWidget = Event.GetScreenSpacePosition() - Geometry.AbsolutePosition;
	MouseLocationOnWidget /= Geometry.Scale;
	uint8 CellX = MouseLocationOnWidget.X / CellSize;
	uint8 CellY = MouseLocationOnWidget.Y / CellSize;
	
	return FCellLocation{ CellX, CellY };
}

FReply FKeshSweeperGameView::OnMouseDown( const FGeometry& Geometry, const FPointerEvent& Event )
{
	if ( Event.GetEffectingButton() == EKeys::LeftMouseButton || Event.GetEffectingButton() == EKeys::RightMouseButton )
	{
		// Start a new click process only if we haven't already had another mouse button clicked
		if ( MouseDownButton == EKeys::Invalid )
		{
			MouseDownButton = Event.GetEffectingButton();
			MouseDownCell = MouseEventToCellLocation( Geometry, Event );
		}
		
		// If we've used multiple buttons, cancel the process
		else
			MouseDownButton = EKeys::Invalid;
	}

	// If we're being funny with mouse buttons, cancel the click process
	else
		MouseDownButton = EKeys::Invalid;

	return FReply::Handled();
}

FReply FKeshSweeperGameView::OnMouseUp( const FGeometry& Geometry, const FPointerEvent& Event )
{
	if ( Event.GetEffectingButton() == EKeys::LeftMouseButton || Event.GetEffectingButton() == EKeys::RightMouseButton )
	{
		// If we're using the same mouse button, continue the click process
		if ( MouseDownButton == Event.GetEffectingButton() )
		{
			FCellLocation MouseUpCell = MouseEventToCellLocation( Geometry, Event );

			// If we've "clicked" the cell
			if ( MouseUpCell.X == MouseDownCell.X && MouseUpCell.Y == MouseDownCell.Y )
			{
				if ( Plugin.IsValid() && Plugin->GetController().IsValid() )
				{
					if ( Event.GetEffectingButton() == EKeys::LeftMouseButton )
						Plugin->GetController()->RevealCell( MouseUpCell );

					else if ( Event.GetEffectingButton() == EKeys::RightMouseButton )
						Plugin->GetController()->SuspectCell( MouseUpCell );
				}
			}
		}
	}

	// Make sure to end the click process
	MouseDownButton = EKeys::Invalid;

	return FReply::Handled();
}

void FKeshSweeperGameView::OnTabClosed( TSharedRef< SDockTab > TabClosed )
{
	// Make sure the tab doesn't hang around because we're referencing it!
	Tab.Reset();
}

void FKeshSweeperGameView::Destruct()
{
	
}

#undef LOCTEXT_NAMESPACE
