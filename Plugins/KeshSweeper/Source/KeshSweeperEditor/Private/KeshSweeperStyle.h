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