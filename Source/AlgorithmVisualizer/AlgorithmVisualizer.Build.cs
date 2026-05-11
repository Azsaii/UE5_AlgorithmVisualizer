// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AlgorithmVisualizer : ModuleRules
{
	public AlgorithmVisualizer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Slate", "SlateCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // 서브 폴더 include 경로 추가
        PublicIncludePaths.AddRange(new string[]
        {
            System.IO.Path.Combine(ModuleDirectory, "Core"),
            System.IO.Path.Combine(ModuleDirectory, "Graph"),
            System.IO.Path.Combine(ModuleDirectory, "Grid"),
            System.IO.Path.Combine(ModuleDirectory, "UI"),
            System.IO.Path.Combine(ModuleDirectory, "Algorithms"),
            System.IO.Path.Combine(ModuleDirectory, "Algorithms/Base"),
            System.IO.Path.Combine(ModuleDirectory, "Algorithms/Pathfinding"),
            System.IO.Path.Combine(ModuleDirectory, "Algorithms/MST")
        });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
