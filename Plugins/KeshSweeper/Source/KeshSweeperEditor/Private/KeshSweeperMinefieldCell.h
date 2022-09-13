// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SOverlay.h"
#include "KeshSweeperGameModel.h"

class SBox;
class SColorBlock;
class SImage;
class STextBlock;

class SKeshSweeperMinefieldCell : public SOverlay
{

public:

	SLATE_BEGIN_ARGS( SKeshSweeperMinefieldCell )
		: _Model( nullptr )
	    , _Loc( { 0, 0 } )
		{ }
		SLATE_ARGUMENT( TSharedPtr< FKeshSweeperGameModel >, Model )
		SLATE_ARGUMENT( FCellLocation, Loc )
	SLATE_END_ARGS()

	SKeshSweeperMinefieldCell();

	void Construct( const FArguments& InArgs );

	const struct FCellLocation& GetCellLocation() const { return Loc; };

	void UpdateDisplay() const;

protected:

	TSharedPtr< FKeshSweeperGameModel > Model;

	FCellLocation Loc;

	TSharedPtr< SColorBlock > Background;
	TSharedPtr< SColorBlock > ExplodedBackground;
	TSharedPtr< SBox > Mine;
	TSharedPtr< STextBlock > NearbyMines;
	TSharedPtr< SImage > Button;
	TSharedPtr< SBox > Suspect;

};
