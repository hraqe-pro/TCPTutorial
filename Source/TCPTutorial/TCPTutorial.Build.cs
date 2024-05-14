// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TCPTutorial : ModuleRules
{
	public TCPTutorial(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Networking", "Sockets" });
	}
}
