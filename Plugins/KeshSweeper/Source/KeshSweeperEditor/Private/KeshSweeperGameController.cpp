// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "KeshSweeperGameController.h"
#include "KeshSweeperEditorModule.h"

FKeshSweeperGameController::FKeshSweeperGameController( TSharedPtr< class FKeshSweeperEditorModule > InPlugin )
{
	Plugin = InPlugin;

	GameStatus = EGameStatus::NotStarted;
	CellsToReveal.Empty();
	TimeSinceAsyncCall = 0.f;
}

void FKeshSweeperGameController::Init()
{
	if ( !Plugin.IsValid() )
	{
		TriggerError();
		return;
	}

	if ( !Plugin->GetModel().IsValid() )
	{
		TriggerError();
		return;
	}

	if ( !Plugin->GetView().IsValid() )
	{
		TriggerError();
		return;
	}
}

void FKeshSweeperGameController::Destruct()
{

}

void FKeshSweeperGameController::OnToolbarButtonClick()
{
	if ( !Plugin.IsValid() )
		return;

	if ( !Plugin->GetModel().IsValid() )
		return;

	if ( !Plugin->GetView().IsValid() )
		return;

	if ( GameStatus == EGameStatus::NotStarted )
	{
		Plugin->GetView()->SetNewMinefieldWidth( DEFAULT_MINEFIELD_WIDTH );
		Plugin->GetView()->SetNewMinefieldHeight( DEFAULT_MINEFIELD_HEIGHT );
		Plugin->GetView()->SetNewMinefieldDifficulty( DEFAULT_MINEFIELD_DIFFICULTY );

		StartNewGame( 
			Plugin->GetView()->GetNewMinefieldWidth(),  
			Plugin->GetView()->GetNewMinefieldHeight(),
			Plugin->GetView()->GetNewMinefieldDifficulty()
		);
	}

	Plugin->GetView()->ShowTab();
}

bool FKeshSweeperGameController::StartNewGame( uint8 Width, uint8 Height, float Difficulty )
{
	if ( GameStatus == EGameStatus::Error )
		return false;

	if ( !Plugin.IsValid() )
		return false;

	if ( !Plugin->GetModel().IsValid() )
		return false;

	if ( !Plugin->GetView().IsValid() )
		return false;

	TArray< bool > Mines = GenerateRandomMinePlacements( 
		Width * Height, 
		GetMineCountForDifficulty( Width * Height, Difficulty )
	);
	
	CellsToReveal.Empty();
	Plugin->GetModel()->InitMinefield( Width, Height, Mines );
	Plugin->GetView()->PopulateMinefield();
	GameStatus = EGameStatus::InProgress;

	// Automatic win? Woo!
	if ( Plugin->GetModel()->GetMineCount() == 0 || 
		 Plugin->GetModel()->GetMineCount() >= Plugin->GetModel()->GetMinefieldSize() )
	{
		TriggerGameEnd( EGameStatus::Won );
	}

	return true;
}

void FKeshSweeperGameController::SuspectCell( const FCellLocation& Loc )
{
	if ( GameStatus != EGameStatus::InProgress )
		return;

	if ( IsRevealingCells() )
		return;

	if ( !Plugin.IsValid() )
	{
		TriggerError();
		return;
	}

	if ( !Plugin->GetModel().IsValid() )
	{
		TriggerError();
		return;
	}

	if ( !Plugin->GetView().IsValid() )
	{
		TriggerError();
		return;
	}

	if ( !Plugin->GetModel()->IsValidLocation( Loc ) )
		return;

	const FCellInfo& CellInfo = Plugin->GetModel()->GetCellInfo( Loc );
	
	switch ( CellInfo.Status )
	{
		case ECellStatus::Hidden:
			if ( Plugin->GetModel()->SuspectCell( Loc ) )
				Plugin->GetView()->UpdateCellDisplay( Loc );
			break;

		case ECellStatus::Suspected:
			if ( Plugin->GetModel()->UnsuspectCell( Loc ) )
				Plugin->GetView()->UpdateCellDisplay( Loc );
			break;
	}
}

void FKeshSweeperGameController::RevealCell( const FCellLocation& Loc )
{
	if ( GameStatus != EGameStatus::InProgress )
		return;

	if ( IsRevealingCells() )
		return;

	if ( !Plugin.IsValid() )
	{
		TriggerError();
		return;
	}

	if ( !Plugin->GetModel().IsValid() )
	{
		TriggerError();
		return;
	}

	if ( !Plugin->GetView().IsValid() )
	{
		TriggerError();
		return;
	}

	if ( !Plugin->GetModel()->IsValidLocation( Loc ) )
		return;

	const FCellInfo& CellInfo = Plugin->GetModel()->GetCellInfo( Loc );

	if ( CellInfo.bIsMine )
	{
		Plugin->GetModel()->ExplodeCell( Loc );
		TriggerGameEnd( EGameStatus::Lost );
		return;
	}

	TimeSinceAsyncCall = 0.f;
	AsyncReveal( Loc );
}

void FKeshSweeperGameController::AsyncReveal( const FCellLocation& Loc )
{
	if ( !Plugin.IsValid() )
	{
		TriggerError();
		return;
	}

	if ( !Plugin->GetModel().IsValid() )
	{
		TriggerError();
		return;
	}

	if ( !Plugin->GetView().IsValid() )
	{
		TriggerError();
		return;
	}

	if ( !Plugin->GetModel()->IsValidLocation( Loc ) )
		return;

	const FCellInfo& CellInfo = Plugin->GetModel()->GetCellInfo( Loc );

	if ( CellInfo.bIsMine )
		return;

	// Did something go wrong revealing the cell? Like it's already revealed?
	if ( !Plugin->GetModel()->RevealCell( Loc ) )
		return;
	
	// Update the cell's graphics
	Plugin->GetView()->UpdateCellDisplay( Loc );

	// Won? Woo!
	if ( ( Plugin->GetModel()->GetMineCount() + Plugin->GetModel()->GetNumberOfCellsRevealed() ) >= Plugin->GetModel()->GetMinefieldSize() )
	{
		TriggerGameEnd( EGameStatus::Won );
		return;
	}

	// Don't reveal other cells if we have nearby mines!
	if ( Plugin->GetModel()->GetNearbyMineCount( Loc ) > 0 )
		return;

	// Add new cells to the reveal queue
	FCellLocation Temp;
	
	// Top
	if ( Loc.Y > 0 )
	{
		Temp = Loc;
		Temp.Y -= 1;
		CellsToReveal.Enqueue( Temp );
	}

	// Left
	if ( Loc.X > 0 )
	{
		Temp = Loc;
		Temp.X -= 1;
		CellsToReveal.Enqueue( Temp );
	}

	// Bottom
	if ( Loc.Y < ( Plugin->GetModel()->GetMinefieldHeight() - 1 ) )
	{
		Temp = Loc;
		Temp.Y += 1;
		CellsToReveal.Enqueue( Temp );
	}

	// Right
	if ( Loc.X < ( Plugin->GetModel()->GetMinefieldWidth() - 1 ) )
	{
		Temp = Loc;
		Temp.X += 1;
		CellsToReveal.Enqueue( Temp );
	}

	// Top Left
	if ( Loc.X > 0 && Loc.Y > 0 )
	{
		Temp = Loc;
		Temp.X -= 1;
		Temp.Y -= 1;
		CellsToReveal.Enqueue( Temp );
	}

	// Top right
	if ( Loc.X < ( Plugin->GetModel()->GetMinefieldWidth() - 1 ) && Loc.Y > 0 )
	{
		Temp = Loc;
		Temp.X += 1;
		Temp.Y -= 1;
		CellsToReveal.Enqueue( Temp );
	}

	// Bottom left
	if ( Loc.X > 0 && Loc.Y < ( Plugin->GetModel()->GetMinefieldHeight() - 1 ) )
	{
		Temp = Loc;
		Temp.X -= 1;
		Temp.Y += 1;
		CellsToReveal.Enqueue( Temp );
	}

	// Bottom right
	if ( Loc.X < ( Plugin->GetModel()->GetMinefieldWidth() - 1 ) && Loc.Y < ( Plugin->GetModel()->GetMinefieldHeight() - 1 ) )
	{
		Temp = Loc;
		Temp.X += 1;
		Temp.Y += 1;
		CellsToReveal.Enqueue( Temp );
	}

	// Double spiral baby
}

void FKeshSweeperGameController::TriggerGameEnd( EGameStatus::Enum NewStatus )
{
	if ( GameStatus != EGameStatus::InProgress )
		return;

	CellsToReveal.Empty();

	if ( !Plugin.IsValid() )
	{
		TriggerError();
		return;
	}

	if ( !Plugin->GetModel().IsValid() )
	{
		TriggerError();
		return;
	}

	if ( !Plugin->GetView().IsValid() )
	{
		TriggerError();
		return;
	}

	Plugin->GetModel()->RevealMinefield();
	GameStatus = NewStatus;
	Plugin->GetView()->UpdateMinefieldDisplay();
}

void FKeshSweeperGameController::TriggerError()
{
	CellsToReveal.Empty();
	GameStatus = EGameStatus::Error;
}

TStatId FKeshSweeperGameController::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT( FKeshSweeperGameController, STATGROUP_Tickables );
}

void FKeshSweeperGameController::Tick( float DeltaTime )
{
	if ( Plugin.IsValid() )
	{
		if ( Plugin->GetView().IsValid() )
		{
			Plugin->GetView()->TickUI();
		}
	}

	if ( GameStatus != EGameStatus::InProgress )
		return;

	if ( CellsToReveal.IsEmpty() )
	{
		TimeSinceAsyncCall = 0.f;
		return;
	}

	static const float TimePerReveal = 0.01f;
	
	TimeSinceAsyncCall += DeltaTime;

	while ( TimeSinceAsyncCall > TimePerReveal )
	{
		FCellLocation NextLocation;
		CellsToReveal.Dequeue( NextLocation );
		AsyncReveal( NextLocation );

		if ( CellsToReveal.IsEmpty() )
		{
			TimeSinceAsyncCall = 0.f;
			break; // Not technically needed, but it's descriptive
		}

		TimeSinceAsyncCall -= TimePerReveal;
	}		
}

uint16 FKeshSweeperGameController::GetMineCountForDifficulty( uint16 CellCount, float Difficulty )
{
	// Some sanity checks
	if ( Difficulty < 1.f )
		Difficulty = 1.f;

	else if ( Difficulty > 10.f )
		Difficulty = 10.f;

	// Mine count goes from grid size^0.5 to grid size^0.9
	float Power = 0.5f + ( ( Difficulty - 1.f ) / 9.f * 0.4f );
	float MineCount = pow( ( float ) CellCount, Power );

	return ( uint16 ) floor( MineCount );
}

TArray< bool > FKeshSweeperGameController::GenerateRandomMinePlacements( uint16 GridSize, uint16 Count )
{
	// Considered implementing the Mersenne Twister for this... but thought that was a little ott!

	TArray< bool > Mines;
	Mines.SetNum( GridSize );

	// "Quick" and dirty
	// Statistically the first and last cells have half the chance of the others.
	// I can live with this.
	for ( int Index = 0; Index < Count; ++Index )
	{
		float RandFloat = ( ( float ) rand() ) / ( ( float ) RAND_MAX );
		uint16 MineIndex = ( uint16 ) round( GridSize * RandFloat );

		// Just in case?
		if ( MineIndex >= GridSize )
			MineIndex = GridSize - 1;

		if ( Mines[ MineIndex ] )
		{
			Index--;
			continue;
		}

		Mines[ MineIndex ] = true;
	}

	return Mines;
}
