// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class IROAS : ModuleRules
{
	public IROAS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "OnlineSubsystem", "OnlineSubsystemSteam", "UMG", "Slate", "SlateCore", "HTTP", "Json", "JsonUtilities", "NavigationSystem", "AIModule", "Niagara", "HeadMountedDisplay", "MovieScene", "LevelSequence", "Chaos", "ChaosCaching", /*"OculusXRAnchors", "OculusXREditor", "OculusXRHMD", "OculusXRInput",*/ "XRBase" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
		PrivateIncludePaths.AddRange(new string[] {
				"IROAS/GameCore",
				"IROAS/GTL",
				"IROAS/YSL",
				"IROAS/DVN",
				"IROAS/GameCore/private",
				"IROAS/GameCore/public",
				"IROAS/DVN/private",
				"IROAS/DVN/public",
				"IROAS/GTL/private",
				"IROAS/GTL/public",
				"IROAS/YSL/private",
				"IROAS/YSL/public"
	});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
