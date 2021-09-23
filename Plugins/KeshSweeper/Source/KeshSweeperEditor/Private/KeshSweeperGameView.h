// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "KeshSweeperGameModel.h"

class FKeshSweeperGameView : public TSharedFromThis< FKeshSweeperGameView >
{

protected:

	static const FName TabName;
	static const TArray<FText> StatusText;
	static const float CellSize;

	static FCellLocation MouseEventToCellLocation( const FGeometry& Geometry, const FPointerEvent& Event );

	TSharedPtr< class FKeshSweeperEditorModule > Plugin;

	TSharedPtr< SDockTab > Tab;
	TSharedPtr< SBox > GameWindow;
	TSharedPtr< SBox > GridContainer;
	TSharedPtr< SCanvas > CellGrid;

	FKey MouseDownButton;
	FCellLocation MouseDownCell;
	uint8 WidthSliderValue;
	uint8 HeightSliderValue;
	float DifficultySliderValue;

	TSharedRef< SDockTab > CreateTab( const FSpawnTabArgs& TabSpawnArgs );
	void OnTabClosed( TSharedRef< SDockTab > );
	void UpdateCellVisual( const FCellLocation& Loc, SOverlay* Cell );

public:

	TSharedPtr< SSpinBox< uint8 > > WidthInput;
	TSharedPtr< SSpinBox< uint8 > > HeightInput;
	TSharedPtr< SSlider > DifficultyInput;

	FKeshSweeperGameView( TSharedPtr< class FKeshSweeperEditorModule > InPlugin );

	void Init();
	void ShowTab();
	void Invalidate(); // Called when the model is changed by the controller
	void InvalidateCell( const FCellLocation& Loc );
	void Destruct();
	void TickUI();
	bool IsTabOpen() const { return Tab.IsValid(); }
	uint8 GetWidthSliderValue() const { return WidthSliderValue; };
	uint8 GetHeightSliderValue() const { return HeightSliderValue; };
	float GetDifficultySliderValue() const { return DifficultySliderValue; };
	void SetWidthSliderValue( uint8 NewVal );
	void SetHeightSliderValue( uint8 NewVal );
	void SetDifficultySliderValue( float NewVal );
	void OnWidthValueChanged( uint8 NewVal );
	void OnHeightValueChanged( uint8 NewVal );
	void OnDifficultyValueChanged( float NewVal );
	TOptional< uint8 > GetMaximumWidth() const;
	TOptional< uint8 > GetMaximumHeight() const;
	FSlateColor GetSliderBarColor() const;
	FText GetMineCount() const;
	FText GetStatusText() const;
	FReply OnNewGameButtonClicked();
	bool PopulateGrid( uint8 Width, uint8 Height );
	FReply OnMouseDown( const FGeometry& Geometry, const FPointerEvent& Event );
	FReply OnMouseUp( const FGeometry& Geometry, const FPointerEvent& Event );

};
