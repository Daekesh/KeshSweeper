// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FSlateStyleSet;

class FKeshSweeperStyle
{

public:

	static void Initialize();
	static void Shutdown();
	static TSharedPtr< FSlateStyleSet > Get();
	static FName GetStyleSetName();

private:

	static TSharedPtr< FSlateStyleSet > StyleSet;

};
