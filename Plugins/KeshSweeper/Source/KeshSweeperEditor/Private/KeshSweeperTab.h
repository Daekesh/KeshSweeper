// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Docking/SDockTab.h"

class FKeshSweeperGameView;
class SBox;
class SButton;
class SCanvas;
class SSlider;
template<typename NumericType>
class SSpinBox;
struct FSlateColor;

class SKeshSweeperTab : public SDockTab
{

public:

	SLATE_BEGIN_ARGS( SKeshSweeperTab )
		: _View( nullptr ) 
		{ }
		SLATE_ARGUMENT( TSharedPtr< FKeshSweeperGameView >, View )
	SLATE_END_ARGS()

	static const FName TabName;

	SKeshSweeperTab();

	void Construct( const FArguments& InArgs );

	TSharedPtr< SSpinBox< uint8 > > GetNewMinefieldWidthSlider() const { return NewMinefieldWidthSlider; };
	TSharedPtr< SSpinBox< uint8 > > GetNewMinefieldHeightSlider() const { return NewMinefieldHeightSlider; };
	TSharedPtr< SSlider > GetNewMinefieldDifficultySlider() const { return NewMinefieldDifficultySlider; };
	TSharedPtr< SButton > GetStartNewGameButton() const { return StartNewGameButton; };
	TSharedPtr< SBox > GetMinefieldContainer() const { return MinefieldContainer; };
	TSharedPtr< SCanvas > GetMinefield() const { return Minefield; };

	void FixWidgets() const;

protected:

	static const TArray< FText > StatusText;

	TSharedPtr< FKeshSweeperGameView > View;

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
