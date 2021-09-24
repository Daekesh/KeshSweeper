// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "KeshSweeperGameModel.h"

class SKeshSweeperMinefieldCell : public SOverlay
{

public:

	SLATE_BEGIN_ARGS( SKeshSweeperMinefieldCell )
		: _Plugin( nullptr )
		, _Loc( { 0, 0 } )
	{

	}

	SLATE_ARGUMENT( TSharedPtr< class FKeshSweeperEditorModule >, Plugin )
	SLATE_ARGUMENT( FCellLocation, Loc )
	SLATE_END_ARGS()

	SKeshSweeperMinefieldCell();

	void Construct( const FArguments& InArgs );
	
	const FCellLocation& GetCellLocation() const { return Loc; }

	void Invalidate();

protected:

	TSharedPtr< class FKeshSweeperEditorModule > Plugin;

	FCellLocation Loc;

	TSharedPtr< SColorBlock > Background;
	TSharedPtr< SColorBlock > ExplodedBackground;
	TSharedPtr< SBox > Mine;
	TSharedPtr< STextBlock > NearbyMines;
	TSharedPtr< SImage > Button;
	TSharedPtr< SBox > Suspect;

};