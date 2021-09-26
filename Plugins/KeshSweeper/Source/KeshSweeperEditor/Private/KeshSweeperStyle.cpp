// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#include "KeshSweeperStyle.h"
#include "SlateBasics.h"

void FKeshSweeperStyle::Initialize()
{
	// Only register once
	if ( StyleSet.IsValid() )
		return;

	StyleSet = MakeShareable( new FSlateStyleSet( GetStyleSetName() ) );

	FSlateStyleRegistry::RegisterSlateStyle( *StyleSet.Get() );

	if ( !StyleSet.IsValid() )
		return;

	// Load editor icons. Make sure they are valid.
	UTexture2D* TargetIcon = LoadObject< UTexture2D >( nullptr, TEXT( "/Engine/EditorMaterials/TargetIcon" ) );
	check( TargetIcon );

	static UTexture2D* ButtonIcon = LoadObject< UTexture2D >( nullptr, TEXT( "/Engine/MobileResources/HUD/MobileHUDButton3.MobileHUDButton3" ) );
	check( ButtonIcon );

	static UTexture2D* MineIcon = LoadObject< UTexture2D >( nullptr, TEXT( "/Engine/MobileResources/HUD/AnalogHat.AnalogHat" ) );
	check( MineIcon );

	static UTexture2D* SuspectIcon = LoadObject< UTexture2D >( nullptr, TEXT( "/Engine/EditorResources/Waypoint.Waypoint" ) );
	check( SuspectIcon );

	StyleSet->Set( "KeshSweeperStyle.OpenWindow", new FSlateImageBrush( TargetIcon, FVector2D( 40.f, 40.f ) ) );
	StyleSet->Set( "KeshSweeperStyle.ButtonBrush", new FSlateImageBrush( ButtonIcon, FVector2D( 64.f, 64.f ) ) );
	StyleSet->Set( "KeshSweeperStyle.MineBrush", new FSlateImageBrush( MineIcon, FVector2D( 32.f, 32.f ) ) );
	StyleSet->Set( "KeshSweeperStyle.SuspectBrush", new FSlateImageBrush( SuspectIcon, FVector2D( 256.f, 256.f ), FLinearColor( 1.f, 0.25f, 0.25f ) ) );
	StyleSet->Set( "KeshSweeperStyle.Background", FLinearColor( 0.5f, 0.5f, 0.5f, 0.5f ) ); // Grey
	StyleSet->Set( "KeshSweeperStyle.ExplodedBackground", FLinearColor( 1.0f, 0.25f, 0.25f ) ); // Red
	StyleSet->Set( "KeshSweeperStyle.MenuText", FCoreStyle::GetDefaultFontStyle( "Regular", 12 ) );
	StyleSet->Set( "KeshSweeperStyle.NearbyMineText", FCoreStyle::GetDefaultFontStyle( "Regular", 12 ) );
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.1", FLinearColor( 0.0f, 0.0f, 0.0f ) ); // 1 Black 
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.2", FLinearColor( 0.0f, 0.0f, 1.0f ) ); // 2 Blue 
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.3", FLinearColor( 0.5f, 1.0f, 1.0f ) ); // 3 Cyan
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.4", FLinearColor( 0.2f, 1.0f, 0.0f ) ); // 4 Green
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.5", FLinearColor( 1.0f, 1.0f, 0.0f ) ); // 5 Yellow
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.6", FLinearColor( 1.0f, 0.6f, 0.2f ) ); // 6 Orange
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.7", FLinearColor( 1.0f, 0.0f, 0.0f ) ); // 7 Red
	StyleSet->Set( "KeshSweeperStyle.NearbyMineTextColour.8", FLinearColor( 1.0f, 0.8f, 0.8f ) );  // 8 Pinkish?
}

void FKeshSweeperStyle::Shutdown()
{
	if ( !StyleSet.IsValid() )
		return;

	FSlateStyleRegistry::UnRegisterSlateStyle( *StyleSet.Get() );
	ensure( StyleSet.IsUnique() );
	StyleSet.Reset();
}

TSharedPtr< class FSlateStyleSet > FKeshSweeperStyle::StyleSet = nullptr;

TSharedPtr< class FSlateStyleSet > FKeshSweeperStyle::Get()
{
	return StyleSet;
}

FName FKeshSweeperStyle::GetStyleSetName()
{
	return TEXT( "KeshSweeperStyle" );
}
