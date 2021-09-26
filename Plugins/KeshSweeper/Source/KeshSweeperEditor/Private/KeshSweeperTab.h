// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "Widgets/Docking/SDockTab.h"

class SKeshSweeperTab : public SDockTab
{

public:

	SLATE_BEGIN_ARGS( SKeshSweeperTab )
		: _View( nullptr )
	{

	}

		SLATE_ARGUMENT( TSharedPtr< class FKeshSweeperGameView >, View )
	SLATE_END_ARGS()

	static const FName TabName;

	SKeshSweeperTab();

	void Construct( const FArguments& InArgs );

	TSharedPtr< SSpinBox< uint8 > > GetNewMinefieldWidthSlider() { return NewMinefieldWidthSlider; };
	TSharedPtr< SSpinBox< uint8 > > GetNewMinefieldHeightSlider() { return NewMinefieldHeightSlider; };
	TSharedPtr< SSlider > GetNewMinefieldDifficultySlider() { return NewMinefieldDifficultySlider; };
	TSharedPtr< SButton > GetStartNewGameButton() { return StartNewGameButton; };
	TSharedPtr< SBox > GetMinefieldContainer() { return MinefieldContainer; };
	TSharedPtr< SCanvas > GetMinefield() { return Minefield; };

	void FixWidgets();

protected:

	static const TArray< FText > StatusText;

	TSharedPtr< class FKeshSweeperGameView > View;

	TSharedPtr< SSpinBox< uint8 > > NewMinefieldWidthSlider;
	TSharedPtr< SSpinBox< uint8 > > NewMinefieldHeightSlider;
	TSharedPtr< SSlider > NewMinefieldDifficultySlider;
	TSharedPtr< SButton > StartNewGameButton;
	TSharedPtr< SBox > GameWindow;
	TSharedPtr< SBox > MinefieldContainer;
	TSharedPtr< SCanvas > Minefield;

	// Slate Getters
	TOptional< uint8 > GetMaximumMinefieldWidth() const;
	TOptional< uint8 > GetMaximumMinefieldHeight() const;
	FSlateColor GetDifficultySliderBarColour() const;
	FText GetNewMinefieldMineCount() const;
	FText GetCurrentGameStatusText() const;

};
