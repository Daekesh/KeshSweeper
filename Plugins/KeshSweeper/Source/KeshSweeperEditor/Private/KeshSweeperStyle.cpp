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
