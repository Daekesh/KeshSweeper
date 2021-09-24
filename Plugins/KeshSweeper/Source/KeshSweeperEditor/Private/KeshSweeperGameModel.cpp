// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "KeshSweeperGameModel.h"
#include "KeshSweeperEditorModule.h"

const FCellInfo FKeshSweeperGameModel::DefaultCellInfo = { false, ECellStatus::Hidden };

FKeshSweeperGameModel::FKeshSweeperGameModel( TSharedPtr< class FKeshSweeperEditorModule > InPlugin )
{
	Plugin = InPlugin;

	Minefield.SetNum( 0 );
	MinefieldWidth = 0;
	MinefieldHeight = 0;
	MineCount = 0;
	CellsRevealed = 0;
}

void FKeshSweeperGameModel::Init()
{

}

void FKeshSweeperGameModel::Destruct()
{

}

void FKeshSweeperGameModel::InitMinefield( uint8 InWidth, uint8 InHeight, TArray< bool > MineLocations )
{
	CellsRevealed = 0;

	// Min grid size is 1x1
	MinefieldWidth = InWidth > 0 ? InWidth : 1;
	MinefieldHeight = InHeight > 0 ? InHeight : 1;

	Minefield.SetNum( ( ( uint16 ) MinefieldWidth ) * ( ( uint16 ) MinefieldHeight ) );
	MineCount = 0;

	// MineLocations's size may not match our cell size. Just being safe.
	for ( int Index = 0; Index < GetMinefieldSize(); ++Index )
	{
		Minefield[ Index ] = FKeshSweeperGameModel::DefaultCellInfo;

		if ( Index < MineLocations.Num() )
		{
			Minefield[ Index ].bIsMine = MineLocations[ Index ];

			if ( MineLocations[ Index ] )
				++MineCount;
		}
	}

	// Must be at least 1 mine
	if ( MineCount == 0 )
	{
		MineCount = 1;
		Minefield[ 0 ].bIsMine = true;
	}
}

FCellLocation FKeshSweeperGameModel::IndexToXY( uint16 CellIndex ) const
{
	return {		
		( uint8 ) ( CellIndex % MinefieldWidth ),
		( uint8 ) ( CellIndex / ( ( uint16 ) MinefieldWidth ) )
	};
}

uint16 FKeshSweeperGameModel::XYToIndex( uint8 CellX, uint8 CellY ) const
{
	return (
		( ( uint16 ) CellX ) +
		( ( uint16 ) MinefieldWidth ) * ( ( uint16 ) CellY )
	);
}

const FCellInfo& FKeshSweeperGameModel::GetCellInfo( uint8 CellX, uint8 CellY ) const
{
	if ( CellX >= MinefieldWidth || CellY >= MinefieldHeight )
		return FKeshSweeperGameModel::DefaultCellInfo;

	uint16 Index = XYToIndex( CellX, CellY );

	if ( Index >= GetMinefieldSize() )
		return FKeshSweeperGameModel::DefaultCellInfo;

	return Minefield[ Index ];
}

uint16 FKeshSweeperGameModel::GetNearbyMineCount( const FCellLocation& Loc ) const
{
	if ( Loc.X >= MinefieldWidth || Loc.Y >= MinefieldHeight )
		return 0;

	uint16 NearbyMineCount = 0;

	// Top left
	if ( Loc.X > 0 && Loc.Y > 0 )
	{
		if ( GetCellInfo( Loc.X - 1, Loc.Y - 1 ).bIsMine )
			++NearbyMineCount;
	}

	// Top 
	if ( Loc.Y > 0 )
	{
		if ( GetCellInfo( Loc.X, Loc.Y - 1 ).bIsMine )
			++NearbyMineCount;
	}

	// Top right
	if ( Loc.X < ( MinefieldWidth - 1 ) && Loc.Y > 0 )
	{
		if ( GetCellInfo( Loc.X + 1, Loc.Y - 1 ).bIsMine )
			++NearbyMineCount;
	}

	// Left
	if ( Loc.X > 0 )
	{
		if ( GetCellInfo( Loc.X - 1, Loc.Y ).bIsMine )
			++NearbyMineCount;
	}

	// Middle
	// It's a me, cellio!

	// Right
	if ( Loc.X < ( MinefieldWidth - 1 ) )
	{
		if ( GetCellInfo( Loc.X + 1, Loc.Y ).bIsMine )
			++NearbyMineCount;
	}

	// Bottom left
	if ( Loc.X > 0 && Loc.Y < ( MinefieldHeight - 1 ) )
	{
		if ( GetCellInfo( Loc.X - 1, Loc.Y + 1 ).bIsMine )
			++NearbyMineCount;
	}

	// Bottom
	if ( Loc.Y < ( MinefieldHeight - 1 ) )
	{
		if ( GetCellInfo( Loc.X, Loc.Y + 1 ).bIsMine )
			++NearbyMineCount;
	}

	// Bottom right
	if ( Loc.X < ( MinefieldWidth - 1 ) && Loc.Y < ( MinefieldHeight - 1 ) )
	{
		if ( GetCellInfo( Loc.X + 1, Loc.Y + 1 ).bIsMine )
			++NearbyMineCount;
	}

	return NearbyMineCount;
}

bool FKeshSweeperGameModel::SuspectCell( const FCellLocation& Loc )
{
	if ( Loc.X >= MinefieldWidth || Loc.Y >= MinefieldHeight )
		return false;

	uint16 Index = XYToIndex( Loc );

	if ( Index >= GetMinefieldSize() )
		return false;

	if ( Minefield[ Index ].Status != ECellStatus::Hidden )
		return false;

	Minefield[ Index ].Status = ECellStatus::Enum::Suspected;
	return true;
}

bool FKeshSweeperGameModel::UnsuspectCell( const FCellLocation& Loc )
{
	if ( Loc.X >= MinefieldWidth || Loc.Y >= MinefieldHeight )
		return false;

	uint16 Index = XYToIndex( Loc );

	if ( Index >= GetMinefieldSize() )
		return false;

	if ( Minefield[ Index ].Status != ECellStatus::Enum::Suspected )
		return false;

	Minefield[ Index ].Status = ECellStatus::Hidden;
	return true;
}

bool FKeshSweeperGameModel::RevealCell( const FCellLocation& Loc )
{
	if ( Loc.X >= MinefieldWidth || Loc.Y >= MinefieldHeight )
		return false;

	uint16 Index = XYToIndex( Loc );

	if ( Index >= GetMinefieldSize() )
		return false;

	if ( Minefield[ Index ].Status == ECellStatus::Revealed )
		return false;

	Minefield[ Index ].Status = ECellStatus::Revealed;

	if ( !Minefield[ Index ].bIsMine )
		++CellsRevealed;

	return true;
}

bool FKeshSweeperGameModel::ExplodeCell( const FCellLocation& Loc )
{
	if ( Loc.X >= MinefieldWidth || Loc.Y >= MinefieldHeight )
		return false;

	uint16 Index = XYToIndex( Loc );

	if ( Index >= GetMinefieldSize() )
		return false;

	if ( Minefield[ Index ].Status == ECellStatus::Exploded )
		return false;

	Minefield[ Index ].Status = ECellStatus::Exploded;
	return true;
}

void FKeshSweeperGameModel::RevealMinefield()
{
	for ( int Index = 0; Index < GetMinefieldSize(); ++Index )
	{
		// Exploded cell is already revealed
		if ( Minefield[ Index ].Status != ECellStatus::Exploded )
			Minefield[ Index ].Status = ECellStatus::Revealed;
	}

	// This is the amount of non-mine cells revealed
	CellsRevealed = ( uint16 ) ( GetMinefieldSize() - MineCount );
}
