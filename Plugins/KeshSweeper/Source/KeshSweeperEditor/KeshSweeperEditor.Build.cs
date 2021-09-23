// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KeshSweeperEditor : ModuleRules
{
	public KeshSweeperEditor( ReadOnlyTargetRules Target ) : base( Target )
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"UnrealEd",
				"KeshSweeperRuntime"
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"InputCore",
				"SlateCore",
				"LevelEditor",
				"EditorStyle"
			}
		);
	}
}
