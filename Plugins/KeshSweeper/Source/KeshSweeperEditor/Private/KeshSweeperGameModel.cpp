#pragma once

#include "KeshSweeperGameModel.h"
#include "KeshSweeperEditorModule.h"

const FCellInfo FKeshSweeperGameModel::DefaultCellInfo = { false, ECellStatus::Hidden };

FKeshSweeperGameModel::FKeshSweeperGameModel( TSharedPtr< class FKeshSweeperEditorModule > InPlugin )
{
	Plugin = InPlugin;
}

void FKeshSweeperGameModel::Init()
{

}

void FKeshSweeperGameModel::Destruct()
{

}

void FKeshSweeperGameModel::SetupCells( uint8 InWidth, uint8 InHeight, TArray<bool> MineLocations )
{
	RevealCount = 0;

	// Min grid size is 1x1
	FieldWidth = InWidth > 0 ? InWidth : 1;
	FieldHeight = InHeight > 0 ? InHeight : 1;

	Cells.SetNum( ( ( uint16 ) FieldWidth ) * ( ( uint16 ) FieldHeight ) );
	MineCount = 0;

	// MineLocations's size may not match our cell size. Just being safe.
	for ( int index = 0; index < Cells.Num(); ++index )
	{
		Cells[ index ] = FKeshSweeperGameModel::DefaultCellInfo;

		if ( index < MineLocations.Num() )
		{
			Cells[ index ].bIsMine = MineLocations[ index ];

			if ( MineLocations[ index ] )
				++MineCount;
		}
	}

	// Must be at least 1 mine
	if ( MineCount == 0 )
	{
		++MineCount;
		Cells[ 0 ].bIsMine = true;
	}
}

FCellLocation FKeshSweeperGameModel::IndexToXY( uint16 CellIndex ) const
{
	return {		
		( uint8 ) ( CellIndex % FieldWidth ),
		( uint8 ) ( CellIndex / ( ( uint16 ) FieldWidth ) )
	};
}

uint16 FKeshSweeperGameModel::XYToIndex( const FCellLocation& Loc ) const
{
	return (
		( ( uint16 ) Loc.X ) + 
		( ( uint16 ) FieldWidth ) * ( ( uint16 ) Loc.Y )
	);
}



const FCellInfo& FKeshSweeperGameModel::GetCellInfo( const FCellLocation& Loc ) const
{
	if ( Loc.X >= FieldWidth || Loc.Y >= FieldHeight )
		return FKeshSweeperGameModel::DefaultCellInfo;

	uint16 index = XYToIndex( Loc );

	if ( index >= Cells.Num() )
		return FKeshSweeperGameModel::DefaultCellInfo;

	return Cells[ index ];
}

const FCellInfo& FKeshSweeperGameModel::GetCellInfo( int CellX, int CellY ) const
{
	return GetCellInfo( { ( uint8 ) CellX, ( uint8 ) CellY } );
}

uint16 FKeshSweeperGameModel::GetNearbyMineCount( const FCellLocation& Loc ) const
{
	if ( Loc.X >= FieldWidth || Loc.Y >= FieldHeight )
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
	if ( Loc.X < ( FieldWidth - 1 ) && Loc.Y > 0 )
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
	if ( Loc.X < ( FieldWidth - 1 ) )
	{
		if ( GetCellInfo( Loc.X + 1, Loc.Y ).bIsMine )
			++NearbyMineCount;
	}

	// Bottom left
	if ( Loc.X > 0 && Loc.Y < ( FieldHeight - 1 ) )
	{
		if ( GetCellInfo( Loc.X - 1, Loc.Y + 1 ).bIsMine )
			++NearbyMineCount;
	}

	// Bottom
	if ( Loc.Y < ( FieldHeight - 1 ) )
	{
		if ( GetCellInfo( Loc.X, Loc.Y + 1 ).bIsMine )
			++NearbyMineCount;
	}

	// Bottom right
	if ( Loc.X < ( FieldWidth - 1 ) && Loc.Y < ( FieldHeight - 1 ) )
	{
		if ( GetCellInfo( Loc.X + 1, Loc.Y + 1 ).bIsMine )
			++NearbyMineCount;
	}

	return NearbyMineCount;
}

bool FKeshSweeperGameModel::SuspectCell( const FCellLocation& Loc )
{
	uint16 index = XYToIndex( Loc );

	if ( index >= Cells.Num() )
		return false;

	if ( Cells[ index ].Status != ECellStatus::Hidden )
		return false;

	Cells[ index ].Status = ECellStatus::Enum::Suspect;
	return true;
}

bool FKeshSweeperGameModel::UnsuspectCell( const FCellLocation& Loc )
{
	uint16 index = XYToIndex( Loc );

	if ( index >= Cells.Num() )
		return false;

	if ( Cells[ index ].Status != ECellStatus::Enum::Suspect )
		return false;

	Cells[ index ].Status = ECellStatus::Hidden;
	return true;
}

bool FKeshSweeperGameModel::RevealCell( const FCellLocation& Loc )
{
	uint16 index = XYToIndex( Loc );

	if ( index >= Cells.Num() )
		return false;

	if ( Cells[ index ].Status == ECellStatus::Revealed )
		return false;

	Cells[ index ].Status = ECellStatus::Revealed;
	++RevealCount;
	return true;
}

bool FKeshSweeperGameModel::ExplodeCell( const FCellLocation& Loc )
{
	uint16 index = XYToIndex( Loc );

	if ( index >= Cells.Num() )
		return false;

	if ( Cells[ index ].Status == ECellStatus::Exploded )
		return false;

	Cells[ index ].Status = ECellStatus::Exploded;
	return true;
}

void FKeshSweeperGameModel::RevealAll()
{
	for ( int index = 0; index < Cells.Num(); ++index )
	{
		// Exploded cell is already revealed
		if ( Cells[ index ].Status != ECellStatus::Exploded )
			Cells[ index ].Status = ECellStatus::Revealed;
	}

	// This is the amount of non-mine cells revealed
	RevealCount = ( uint16 ) ( Cells.Num() - MineCount );
}
