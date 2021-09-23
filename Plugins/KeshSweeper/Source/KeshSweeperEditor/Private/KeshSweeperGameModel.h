// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"

// Avoid namespace pollution
namespace ECellStatus
{
	enum Enum
	{
		Hidden,
		Suspect,
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

protected:

	uint8 FieldWidth;
	uint8 FieldHeight;
	uint16 MineCount; // Could be 65k mines? Worst case scenario.
	TArray< FCellInfo > Cells;
	uint16 RevealCount;

	TSharedPtr< class FKeshSweeperEditorModule > Plugin;

public:

	static const FCellInfo DefaultCellInfo;

	FKeshSweeperGameModel( TSharedPtr< class FKeshSweeperEditorModule > InPlugin );

	void Init();
	void Destruct();

	uint8 GetFieldWidth() const { return FieldWidth; }
	uint8 GetFieldHeight() const { return FieldHeight; }
	uint16 GetMineCount() const { return MineCount; }
	uint16 GetCellCount() const { return ( uint16 ) Cells.Num(); }
	uint16 GetRevealCount() const { return RevealCount; }

	void SetupCells( uint8 InWidth, uint8 InHeight, TArray<bool> MineLocations );

	// 0-based indexes, on both.
	FCellLocation IndexToXY( uint16 CellIndex ) const;
	uint16 XYToIndex( const FCellLocation& Loc ) const;
	const FCellInfo& GetCellInfo( const FCellLocation& Loc ) const;
	const FCellInfo& GetCellInfo( int CellX, int CellY ) const;
	uint16 GetNearbyMineCount( const FCellLocation& Loc ) const;
	bool SuspectCell( const FCellLocation& Loc );
	bool UnsuspectCell( const FCellLocation& Loc );
	bool RevealCell( const FCellLocation& Loc );
	bool ExplodeCell( const FCellLocation& Loc );
	void RevealAll();

};
