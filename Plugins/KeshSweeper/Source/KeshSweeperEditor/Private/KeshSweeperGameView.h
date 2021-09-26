// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"

class FKeshSweeperGameView : public TSharedFromThis< FKeshSweeperGameView >
{

public:

	// Width and height of the minefield cells (in slate units.)
	static const float CellSize; 

	FKeshSweeperGameView( TSharedPtr< class FKeshSweeperEditorModule > InPlugin );

	void Init();
	void Destruct();

	TSharedPtr< class FKeshSweeperEditorModule > GetPlugin() { return Plugin; };

	void ShowTab();
	bool IsTabOpen() const { return Tab.IsValid(); }
	void OnTabClosed( TSharedRef< SDockTab > TabClosed );

	// Values for new games
	uint8 GetNewMinefieldWidth() const { return NewMinefieldWidth; };
	uint8 GetNewMinefieldHeight() const { return NewMinefieldHeight; };
	float GetNewMinefieldDifficulty() const { return NewMinefieldDifficulty; };
	
	// Called externally to set the new values
	void SetNewMinefieldWidth( uint8 NewVal );
	void SetNewMinefieldHeight( uint8 NewVal );
	void SetNewMinefieldDifficulty( float NewVal );

	// UI callbacks to update internal values
	void OnNewMinefieldWidthSliderValueChanged( uint8 NewVal );
	void OnNewMinefieldHeightSliderValueChanged( uint8 NewVal );
	void OnNewMinefieldDifficultySliderValueChanged( float NewVal );

	// Discards the current minefield and creates a new one based on the Model, not the sliders.
	bool PopulateMinefield();

	// Updates the display of a single cell
	void UpdateCellDisplay( const struct FCellLocation& Loc );

	// Updates the display for the entire minefield
	void UpdateMinefieldDisplay();

	void TickUI();

protected:

	static FCellLocation MouseEventToCellLocation( const FGeometry& Geometry, const FPointerEvent& Event );

	TSharedPtr< class FKeshSweeperEditorModule > Plugin;
	TSharedPtr< class SKeshSweeperTab > Tab;

	FKey MouseDownButton;
	FCellLocation MouseDownCell;

	// Authoritative values for the sliders
	uint8 NewMinefieldWidth;
	uint8 NewMinefieldHeight;
	float NewMinefieldDifficulty;

	TSharedRef< SDockTab > CreateTab( const FSpawnTabArgs& TabSpawnArgs );

	// UI interactions
	FReply OnNewGameButtonClicked();
	FReply OnMinefieldMouseButtonDown( const FGeometry& Geometry, const FPointerEvent& Event );
	FReply OnMinefieldMouseButtonUp( const FGeometry& Geometry, const FPointerEvent& Event );
	void OnMinefieldClicked( FKey MouseButton, const struct FCellLocation& Cell );

};
