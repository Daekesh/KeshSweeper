// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TickableEditorObject.h"
#include "Containers/Queue.h"

class FKeshSweeperEditorModule;
struct FCellLocation;

// Avoid namespace pollution
namespace EGameStatus
{
	enum Enum
	{
		NotStarted,
		InProgress,
		Lost,
		Won,
		Error
	};
}

class FKeshSweeperGameController : FTickableEditorObject
{

public:

	static uint16 GetMineCountForDifficulty( uint16 CellCount, float Difficulty );

	FKeshSweeperGameController();

	void Init();
	void Destruct();

	TSharedPtr< FKeshSweeperEditorModule > GetPlugin();
	EGameStatus::Enum GetGameStatus() const { return GameStatus; }

	void OnToolbarButtonClick();
	bool StartNewGame( uint8 Width, uint8 Height, float Difficulty );

	// Called from the View when cells are clicked
	void SuspectCell( const FCellLocation& Loc );
	void RevealCell( const FCellLocation& Loc );

	// If this returns true, UI signals are not processed.
	bool IsRevealingCells() const { return !CellsToReveal.IsEmpty(); }

	/* FTickableEditorObject override */
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual bool IsTickable() const override { return true; }
	virtual TStatId GetStatId() const override;
	virtual void Tick( float DeltaTime ) override;
	/* end */

protected:

	static TArray< bool > GenerateRandomMinePlacements( uint16 GridSize, uint16 Count );

	EGameStatus::Enum GameStatus;
	TQueue< FCellLocation > CellsToReveal;
	float TimeSinceAsyncCall;

	// Triggers either a win or a loss.
	void TriggerGameEnd( EGameStatus::Enum );
	
	// Shuts down all control functions. The UI still works, but signals aren't processed
	void TriggerError();

	// Called by the tick method to reveal a single cell
	// Also adds more cells to the reveal queue based on the surrounding cells
	void AsyncReveal( const FCellLocation& Loc );

};
