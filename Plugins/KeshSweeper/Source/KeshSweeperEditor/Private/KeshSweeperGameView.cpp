// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "KeshSweeperGameView.h"
#include "KeshSweeperEditorModule.h"
#include "KeshSweeperGameModel.h"
#include "KeshSweeperStyle.h"
#include "KeshSweeperTab.h"
#include "KeshSweeperMinefieldCell.h"

#define LOCTEXT_NAMESPACE "FKeshSweeperEditorModule"

const float FKeshSweeperGameView::CellSize = 32.f;

FKeshSweeperGameView::FKeshSweeperGameView( TSharedPtr< class FKeshSweeperEditorModule > InPlugin )
{
	Plugin = InPlugin;

	// Default slider values
	NewMinefieldWidth = 1;
	NewMinefieldHeight = 1;
	NewMinefieldDifficulty = 1.f;

	// Minefield click detection
	MouseDownButton = EKeys::Invalid;
	MouseDownCell.X = -1;
	MouseDownCell.Y = -1;
}

void FKeshSweeperGameView::Init()
{

	// Register the tab spawner so for the button.
	TSharedRef< FGlobalTabmanager > TabManager = FGlobalTabmanager::Get();
	TabManager->RegisterTabSpawner( SKeshSweeperTab::TabName, FOnSpawnTab::CreateRaw( this, &FKeshSweeperGameView::CreateTab ) );
}

void FKeshSweeperGameView::Destruct()
{
	TSharedRef< FGlobalTabmanager > TabManager = FGlobalTabmanager::Get();
	TabManager->UnregisterTabSpawner( SKeshSweeperTab::TabName );
}

TSharedRef<SDockTab> FKeshSweeperGameView::CreateTab( const FSpawnTabArgs& TabSpawnArgs )
{
	// Create the tab layout
	TSharedRef< SKeshSweeperTab > NewTab = SNew( SKeshSweeperTab ).View( Plugin->GetView() ); // Avoid making a new shared ptr.

	if ( NewTab->GetStartNewGameButton().IsValid() )
		NewTab->GetStartNewGameButton()->SetOnClicked( FOnClicked::CreateSP( this, &FKeshSweeperGameView::OnNewGameButtonClicked ) );

	if ( NewTab->GetMinefield().IsValid() )
	{
		// Hook in the "button" process
		NewTab->GetMinefield()->SetOnMouseButtonDown( FPointerEventHandler::CreateSP( this, &FKeshSweeperGameView::OnMinefieldMouseButtonDown ) );
		NewTab->GetMinefield()->SetOnMouseButtonUp( FPointerEventHandler::CreateSP( this, &FKeshSweeperGameView::OnMinefieldMouseButtonUp ) );
	}

	return NewTab;
}

void FKeshSweeperGameView::ShowTab()
{
	TSharedRef< FGlobalTabmanager > TabManager = FGlobalTabmanager::Get();
	Tab = StaticCastSharedPtr< SKeshSweeperTab >( TabManager->TryInvokeTab( SKeshSweeperTab::TabName ) );
	
	if ( !Tab.IsValid() )
		return;

	PopulateMinefield();
	UpdateMinefieldDisplay();
}

void FKeshSweeperGameView::OnTabClosed( TSharedRef< SDockTab > TabClosed )
{
	// Make sure the tab doesn't hang around because we're referencing it!
	Tab.Reset();
}

void FKeshSweeperGameView::SetNewMinefieldWidth( uint8 NewVal )
{
	NewMinefieldWidth = NewVal;

	if ( !Tab.IsValid() )
		return;

	if ( !Tab->GetNewMinefieldWidthSlider().IsValid() )
		return;

	Tab->GetNewMinefieldWidthSlider()->SetValue( NewVal );
}

void FKeshSweeperGameView::SetNewMinefieldHeight( uint8 NewVal )
{
	NewMinefieldHeight = NewVal;

	if ( !Tab.IsValid() )
		return;

	if ( !Tab->GetNewMinefieldHeightSlider().IsValid() )
		return;

	Tab->GetNewMinefieldHeightSlider()->SetValue( NewVal );
}

void FKeshSweeperGameView::SetNewMinefieldDifficulty( float NewVal )
{
	NewMinefieldDifficulty = NewVal;

	if ( !Tab.IsValid() )
		return;

	if ( !Tab->GetNewMinefieldDifficultySlider().IsValid() )
		return;

	Tab->GetNewMinefieldDifficultySlider()->SetValue( NewVal );

}

void FKeshSweeperGameView::OnNewMinefieldWidthSliderValueChanged( uint8 NewVal )
{
	NewMinefieldWidth = NewVal;
}

void FKeshSweeperGameView::OnNewMinefieldHeightSliderValueChanged( uint8 NewVal )
{
	NewMinefieldHeight = NewVal;
}

void FKeshSweeperGameView::OnNewMinefieldDifficultySliderValueChanged( float NewVal )
{
	NewMinefieldDifficulty = NewVal;
}

FReply FKeshSweeperGameView::OnNewGameButtonClicked()
{
	if ( !Plugin.IsValid() )
		return FReply::Handled();

	if ( !Plugin->GetController().IsValid() )
		return FReply::Handled();

	Plugin->GetController()->StartNewGame( NewMinefieldWidth, NewMinefieldHeight, NewMinefieldDifficulty );
	
	return FReply::Handled();
}

bool FKeshSweeperGameView::PopulateMinefield()
{
	if ( !Plugin.IsValid() )
		return false;

	if ( !Plugin->GetModel().IsValid() )
		return false;

	if ( !Tab.IsValid() )
		return false;
	
	if ( !Tab->GetMinefieldContainer().IsValid() )
		return false;

	if ( !Tab->GetMinefield().IsValid() )
		return false;

	Tab->GetMinefieldContainer()->SetWidthOverride( Plugin->GetModel()->GetMinefieldWidth() * FKeshSweeperGameView::CellSize );
	Tab->GetMinefieldContainer()->SetHeightOverride( Plugin->GetModel()->GetMinefieldHeight() * FKeshSweeperGameView::CellSize );

	Tab->GetMinefield()->ClearChildren();
	
	for ( uint8 HeightIndex = 0; HeightIndex < Plugin->GetModel()->GetMinefieldHeight(); ++HeightIndex )
	{
		for ( uint8 WidthIndex = 0; WidthIndex < Plugin->GetModel()->GetMinefieldWidth(); ++WidthIndex )
		{
			Tab->GetMinefield()->AddSlot()
				.Position( FVector2D( WidthIndex * FKeshSweeperGameView::CellSize, HeightIndex * FKeshSweeperGameView::CellSize ) )
				.Size( FVector2D( FKeshSweeperGameView::CellSize, FKeshSweeperGameView::CellSize ) )
				[
					SNew( SKeshSweeperMinefieldCell )
						.Model( Plugin->GetModel() )
						.Loc( { WidthIndex, HeightIndex } )
				];
		}
	}

	// Update the visuals of every cell
	UpdateMinefieldDisplay();

	return true;
}

FCellLocation FKeshSweeperGameView::MouseEventToCellLocation( const FGeometry& Geometry, const FPointerEvent& Event )
{
	FVector2D MouseLocationOnWidget = Event.GetScreenSpacePosition() - Geometry.AbsolutePosition;
	
	// Unscale position so it is on the same scale as the base "CellSize"
	MouseLocationOnWidget /= Geometry.Scale; 

	uint8 CellX = MouseLocationOnWidget.X / FKeshSweeperGameView::CellSize;
	uint8 CellY = MouseLocationOnWidget.Y / FKeshSweeperGameView::CellSize;
	
	return { CellX, CellY };
}

FReply FKeshSweeperGameView::OnMinefieldMouseButtonDown( const FGeometry& Geometry, const FPointerEvent& Event )
{
	// If we're being funny with mouse buttons, abort the click process.
	if ( Event.GetEffectingButton() != EKeys::LeftMouseButton && 
		 Event.GetEffectingButton() != EKeys::RightMouseButton )
	{
		MouseDownButton = EKeys::Invalid;
	}

	// If we've already started the click process, abort it.
	else if ( MouseDownButton != EKeys::Invalid )
		MouseDownButton = EKeys::Invalid;

	// Start a new click process
	else
	{
		MouseDownButton = Event.GetEffectingButton();
		MouseDownCell = FKeshSweeperGameView::MouseEventToCellLocation( Geometry, Event );
	}

	return FReply::Handled();
}

FReply FKeshSweeperGameView::OnMinefieldMouseButtonUp( const FGeometry& Geometry, const FPointerEvent& Event )
{
	// If we're being funny with mouse buttons, abort the click process.
	if ( Event.GetEffectingButton() != EKeys::LeftMouseButton &&
		 Event.GetEffectingButton() != EKeys::RightMouseButton )
	{
		;
	}

	// Started the click process with a different button, abort it.
	else if ( MouseDownButton != Event.GetEffectingButton() )
		;

	// Continue the click process
	else
	{
		FCellLocation MouseUpCell = FKeshSweeperGameView::MouseEventToCellLocation( Geometry, Event );

		// If we've "clicked" the cell
		if ( MouseUpCell.X == MouseDownCell.X && MouseUpCell.Y == MouseDownCell.Y )
			OnMinefieldClicked( Event.GetEffectingButton(), MouseUpCell );

	}

	// Make sure to end the click process
	MouseDownButton = EKeys::Invalid;

	return FReply::Handled();
}

void FKeshSweeperGameView::OnMinefieldClicked( FKey MouseButton, const FCellLocation& Cell )
{
	if ( !Plugin.IsValid() )
		return;

	if ( !Plugin->GetModel().IsValid() )
		return;

	if ( !Plugin->GetModel()->IsValidLocation( Cell ) )
		return;

	if ( !Plugin->GetController().IsValid() )
		return;
	
	if ( MouseButton == EKeys::LeftMouseButton )
		Plugin->GetController()->RevealCell( Cell );

	else if ( MouseButton == EKeys::RightMouseButton )
		Plugin->GetController()->SuspectCell( Cell );
}

void FKeshSweeperGameView::UpdateMinefieldDisplay()
{
	if ( !Tab.IsValid() )
		return;
	
	if ( !Tab->GetMinefield().IsValid() )
		return;

	// Get the minefield cells
	FChildren* Cells = Tab->GetMinefield()->GetChildren();

	// Iterate over them
	for ( int Index = 0; Index < Cells->Num(); ++Index )
	{
		// Get the child at the given index
		TSharedRef< SKeshSweeperMinefieldCell > Child = StaticCastSharedRef< SKeshSweeperMinefieldCell >( Cells->GetChildAt( Index ) );
		
		// Update the cell graphic
		Child->UpdateDisplay();
	}
}

void FKeshSweeperGameView::UpdateCellDisplay( const FCellLocation& Loc )
{
	if ( !Plugin.IsValid() )
		return;

	if ( !Plugin->GetModel().IsValid() )
		return;

	if ( !Plugin->GetModel()->IsValidLocation( Loc ) )
		return;

	if ( !Tab.IsValid() )
		return;

	if ( !Tab->GetMinefield().IsValid() )
		return;

	int Index = Plugin->GetModel()->XYToIndex( Loc );

	// Get the grid cells
	FChildren* Cells = Tab->GetMinefield()->GetChildren();

	// Get the child at the given index
	TSharedRef< SKeshSweeperMinefieldCell > Child = StaticCastSharedRef< SKeshSweeperMinefieldCell >( Cells->GetChildAt( Index ) );

	// Update the cell graphic
	Child->UpdateDisplay();
}

void FKeshSweeperGameView::TickUI()
{
	if ( !IsTabOpen() )
		return;

	if ( !Tab.IsValid() )
		return;

	Tab->FixWidgets();
}

#undef LOCTEXT_NAMESPACE
