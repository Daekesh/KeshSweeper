// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FKeshSweeperStyle
{

public:

	static void Initialize();
	static void Shutdown();
	static TSharedPtr< class FSlateStyleSet > Get();
	static FName GetStyleSetName();

private:

	static TSharedPtr< class FSlateStyleSet > StyleSet;

};