// Copyright Matthew "Daekesh" Chapman (c) 1983-2021. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class BlankProjectEditorTarget : TargetRules
{
	public BlankProjectEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "BlankProject" } );
	}
}
