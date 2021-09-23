#pragma once

#include "CoreMinimal.h"
#include "KeshSweeperGameModel.h"

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

protected:

	// Would have used static variables, but can't with floats!
	static uint8 DefaultWidth() { return 20; };
	static uint8 DefaultHeight() { return 20; };
	static float DefaultDifficulty() { return 4.f; };

	static TArray< bool > GenerateRandomMinePlacements( uint16 GridSize, uint16 Count );
	
	TSharedPtr< class FKeshSweeperEditorModule > Plugin;
	EGameStatus::Enum GameStatus;
	TQueue< FCellLocation > CellsToReveal;
	float TimeSinceAsyncCall;

	void TriggerGameEnd( EGameStatus::Enum );
	void TriggerError();
	void AsyncReveal( const FCellLocation& Loc );

public:

	FKeshSweeperGameController( TSharedPtr< class FKeshSweeperEditorModule > InPlugin );

	void Init();
	void Destruct();

	uint16 GetMineCountForDifficulty( float Difficulty ) const;
	EGameStatus::Enum GetGameStatus() const { return GameStatus; }
	void InitiateGame();
	bool StartNewGame( uint8 Width, uint8 Height, float Difficulty );
	void SuspectCell( const FCellLocation& Loc );
	void RevealCell( const FCellLocation& Loc );
	bool IsRevealingCells() const { return !CellsToReveal.IsEmpty(); }

	/* FTickableEditorObject override */
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual bool IsTickable() const override { return true; }
	virtual TStatId GetStatId() const override;
	virtual void Tick( float DeltaTime ) override;
	/* end */
};
