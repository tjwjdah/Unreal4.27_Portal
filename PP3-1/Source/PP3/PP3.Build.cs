// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PP3 : ModuleRules
{
	public PP3(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "UMG", "Niagara", "SlateCore", "RHI", "RenderCore" });
	}
}
