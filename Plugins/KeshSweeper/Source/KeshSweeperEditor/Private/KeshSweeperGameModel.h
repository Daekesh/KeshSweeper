// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"

// Avoid namespace pollution
namespace ECellStatus
{
	enum Enum
	{
		Hidden,
		Suspected,
		Revealed,
		Exploded
	};
}

struct FCellInfo
{
	bool bIsMine;
	ECellStatus::Enum Status;
};

struct FCellLocation
{
	uint8 X;
	uint8 Y;
};

class FKeshSweeperGameModel
{

public:

	static const FCellInfo DefaultCellInfo;

	FKeshSweeperGameModel( TSharedPtr< class FKeshSweeperEditorModule > InPlugin );

	void Init();
	void Destruct();

	// Minefield information
	uint16 GetMinefieldSize() const { return ( uint16 ) Minefield.Num(); }
	uint8 GetMinefieldWidth() const { return MinefieldWidth; }
	uint8 GetMinefieldHeight() const { return MinefieldHeight; }
	uint16 GetMineCount() const { return MineCount; }
	uint16 GetNumberOfCellsRevealed() const { return CellsRevealed; }

	// Creates a new minefield
	void InitMinefield( uint8 InWidth, uint8 InHeight, TArray< bool > MineLocations );

	// 0-based index helper functions
	FCellLocation IndexToXY( uint16 CellIndex ) const;
	uint16 XYToIndex( const FCellLocation& Loc ) const { return XYToIndex( Loc.X, Loc.Y ); };
	uint16 XYToIndex( uint8 CellX, uint8 CellY ) const;

	// Minefield cell information
	const FCellInfo& GetCellInfo( uint8 CellX, uint8 CellY ) const;
	const FCellInfo& GetCellInfo( const FCellLocation& Loc ) const { return GetCellInfo( Loc.X, Loc.Y ); };
	uint16 GetNearbyMineCount( const FCellLocation& Loc ) const;
	
	// Update the model
	bool SuspectCell( const FCellLocation& Loc );
	bool UnsuspectCell( const FCellLocation& Loc );
	bool RevealCell( const FCellLocation& Loc );
	bool ExplodeCell( const FCellLocation& Loc );
	void RevealMinefield();

protected:

	TSharedPtr< class FKeshSweeperEditorModule > Plugin;

	TArray< FCellInfo > Minefield;
	uint8 MinefieldWidth;
	uint8 MinefieldHeight;

	// Could be 65k mines? Worst case scenario.
	uint16 MineCount; 

	// The number of non-mine cells revealed.
	// Is *not* increased when a mine is revealed/exploded.
	uint16 CellsRevealed;

};
