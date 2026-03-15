// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BalloonKeepUp : ModuleRules
{
	public BalloonKeepUp(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"BalloonKeepUp",
			"BalloonKeepUp/Variant_Platforming",
			"BalloonKeepUp/Variant_Platforming/Animation",
			"BalloonKeepUp/Variant_Combat",
			"BalloonKeepUp/Variant_Combat/AI",
			"BalloonKeepUp/Variant_Combat/Animation",
			"BalloonKeepUp/Variant_Combat/Gameplay",
			"BalloonKeepUp/Variant_Combat/Interfaces",
			"BalloonKeepUp/Variant_Combat/UI",
			"BalloonKeepUp/Variant_SideScrolling",
			"BalloonKeepUp/Variant_SideScrolling/AI",
			"BalloonKeepUp/Variant_SideScrolling/Gameplay",
			"BalloonKeepUp/Variant_SideScrolling/Interfaces",
			"BalloonKeepUp/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
