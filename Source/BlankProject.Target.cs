// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class BlankProjectTarget : TargetRules
{
	public BlankProjectTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "BlankProject" } );
	}
}
