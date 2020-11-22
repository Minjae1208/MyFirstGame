// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NProject : ModuleRules
{
	public NProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //PublicIncludePaths.AddRange(new string[] { });
        //PublicIncludePaths.AddRange(new string[] { "" });
        PublicIncludePaths.Add("NProject");

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",

            "UMG" });
	}
}
