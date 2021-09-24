// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "KeshSweeperGameModel.h"
#include "KeshSweeperStyle.h"
#include "KeshSweeperMinefieldCell.h"

class FKeshSweeperGameView : public TSharedFromThis< FKeshSweeperGameView >
{

public:

	// Width and height of the minefield cells (in slate units.)
	static const float CellSize; 

	FKeshSweeperGameView( TSharedPtr< class FKeshSweeperEditorModule > InPlugin );

	void Init();
	void Destruct();

	void ShowTab();
	bool IsTabOpen() const { return Tab.IsValid(); }

	// Values for new games
	uint8 GetNewMinefieldWidth() const { return NewMinefieldWidth; };
	uint8 GetNewMinefieldHeight() const { return NewMinefieldHeight; };
	float GetNewMinefieldDifficulty() const { return NewMinefieldDifficulty; };
	
	// Called externally to set the new values
	void SetNewMinefieldWidth( uint8 NewVal );
	void SetNewMinefieldHeight( uint8 NewVal );
	void SetNewMinefieldDifficulty( float NewVal );

	// Discards the current minefield and creates a new one based on the Model, not the sliders.
	bool PopulateMinefield();

	// Updates the display of a single cell
	void InvalidateCell( const FCellLocation& Loc );

	// Updates the display for the entire minefield
	void InvalidateMinefield();

	void TickUI();

protected:

	static const FName TabName;
	static const TArray< FText > StatusText;

	static FCellLocation MouseEventToCellLocation( const FGeometry& Geometry, const FPointerEvent& Event );

	TSharedPtr< class FKeshSweeperEditorModule > Plugin;

	TSharedPtr< SDockTab > Tab;
	TSharedPtr< SSpinBox< uint8 > > NewMinefieldWidthSlider;
	TSharedPtr< SSpinBox< uint8 > > NewMinefieldHeightSlider;
	TSharedPtr< SSlider > NewMinefieldDifficultySlider;
	TSharedPtr< SBox > GameWindow;
	TSharedPtr< SBox > MinefieldContainer;
	TSharedPtr< SCanvas > Minefield;

	FKey MouseDownButton;
	FCellLocation MouseDownCell;

	// Authoritative values for the sliders
	uint8 NewMinefieldWidth;
	uint8 NewMinefieldHeight;
	float NewMinefieldDifficulty;

	TSharedRef< SDockTab > CreateTab( const FSpawnTabArgs& TabSpawnArgs );
	void OnTabClosed( TSharedRef< SDockTab > );

	// UI callbacks to update internal values
	void OnNewMinefieldWidthChanged( uint8 NewVal );
	void OnNewMinefieldHeightChanged( uint8 NewVal );
	void OnNewMinefieldDifficultyChanged( float NewVal );

	// Slate Getters
	TOptional< uint8 > GetMaximumMinefieldWidth() const;
	TOptional< uint8 > GetMaximumMinefieldHeight() const;
	FSlateColor GetDifficultySliderBarColour() const;
	FText GetNewMinefieldMineCount() const;
	FText GetCurrentGameStatusText() const;

	// UI interactions
	FReply OnNewGameButtonClicked();
	FReply OnMinefieldMouseButtonDown( const FGeometry& Geometry, const FPointerEvent& Event );
	FReply OnMinefieldMouseButtonUp( const FGeometry& Geometry, const FPointerEvent& Event );
	void OnMinefieldClicked( FKey MouseButton, const FCellLocation& Cell );

};
